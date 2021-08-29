#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <gnl_ternary_search_tree_t.h>
#include <gnl_logger.h>
#include "./gnl_simfs_file_descriptor_table.c"
#include "./gnl_simfs_inode.c"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
struct gnl_simfs_file_system {

    // the file system file table, contains all the inodes of the
    // files present into the file system
    struct gnl_ternary_search_tree_t *file_table;

    // the allocated memory in bytes, it should be used
    // to compare the occupied memory size with the
    // memory limit set for the file system
    unsigned long heap_size;

    // the counter of the files present into the file system
    int files_count;

    // the number of files that can be handled by the file system
    int files_limit;

    // the memory allocable in bytes by the file system
    unsigned long memory_limit;

    // contains all the open files in a precisely time,
    // the index is the file descriptor, the value is a
    // copy of the inode of the file.
    struct gnl_simfs_file_descriptor_table *file_descriptor_table;

    // the lock of the file system
    pthread_mutex_t mtx;

    // the logger instance to use for logging
    struct gnl_logger *logger;
};

/**
 * Destroy an inode. It should be passed to the gnl_ternary_search_tree_destroy
 * method of the gnl_ternary_search_tree data structure.
 *
 * @param ptr   The void pointer value of the gnl_ternary_search_tree data structure.
 */
static void destroy_file_table_inode(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    // implicitly cast the value
    struct gnl_simfs_inode *inode = ptr;

    // destroy the obtained inode
    gnl_simfs_inode_destroy(inode);
}

/**
 * Create a new file and put it into the given file system.
 *
 * @param file_system   The file system instance where to put the created file.
 * @param filename      The filename of the file to create.
 *
 * @return              Returns the inode of the created file on success,
 *                      NULL otherwise.
 */
static struct gnl_simfs_inode *file_table_create(struct gnl_simfs_file_system *file_system, const char *filename) {

    gnl_logger_debug(file_system->logger, "Creating file: \"%s\"", filename);

    // check if we can create a new file
    if (file_system->files_count == file_system->files_limit) {
        errno = EDQUOT;
        return NULL;
    }

    // check if there is enough memory
    if (file_system->heap_size == file_system->memory_limit) {
        errno = EDQUOT;
        return NULL;
    }

    // create a new inode
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init(filename);
    GNL_NULL_CHECK(inode, errno, NULL)

    // put the inode into the file table
    int res = gnl_ternary_search_tree_put(&file_system->file_table, filename, inode);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // increment the files counter
    file_system->files_count++;

    gnl_logger_debug(file_system->logger, "Created file \"%s\"", filename);

    return inode;
}

/**
 * Update an existing file table entry with the given buffer entry.
 *
 * @param file_system   The file system instance where the file table resides.
 * @param key           The key of the entry to be updated.
 * @param buffer_entry     The new entry to use to update the existing entry.
 * @param count         The count of bytes eventually wrote in the file within
 *                      the given buffer_entry inode since it was instantiate.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int file_table_fflush(struct gnl_simfs_file_system *file_system, const char *key,
        const struct gnl_simfs_inode *buffer_entry, size_t count) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(key, EINVAL, -1)
    GNL_NULL_CHECK(buffer_entry, EINVAL, -1)

    gnl_logger_debug(file_system->logger, "Updating entry \"%s\" in the file table", key);

    // search the key in the file table
    void *raw_inode = gnl_ternary_search_tree_get(file_system->file_table, key);

    // if the key is not present return an error
    if (raw_inode == NULL) {
        gnl_logger_debug(file_system->logger, "Entry \"%s\" not found, returning with error", key);
        errno = EINVAL;

        return -1;
    }

    gnl_logger_debug(file_system->logger, "Entry \"%s\" found, updating", key);

    // else cast the raw_inode
    struct gnl_simfs_inode *inode = (struct gnl_simfs_inode *)raw_inode;

    // update the inode with the new entry
    int res = gnl_simfs_inode_update(inode, buffer_entry, count);
    if (res == -1) {
        gnl_logger_debug(file_system->logger, "Update on entry \"%s\" failed: %s", key, strerror(errno));

        //let the errno bubble

        return -1;
    }

    // update the file system
    file_system->heap_size += count;

    // if count > 0, provide details about the heap
    if (count > 0) {
        gnl_logger_debug(file_system->logger, "Update on entry \"%s\" succeeded, the heap size has now %d bytes left",
                         key, file_system->memory_limit - file_system->heap_size);
    } else {
        gnl_logger_debug(file_system->logger, "Update on entry \"%s\" succeeded", key);
    }

    return 0;
}

/**
 * Remove an existing file table entry.
 *
 * @param file_system   The file system instance where the file table resides.
 * @param key           The key of the entry to be removed.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int file_table_remove(struct gnl_simfs_file_system *file_system, const char *key) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(key, EINVAL, -1)

    gnl_logger_debug(file_system->logger, "Removing entry \"%s\" from the file table", key);

    // search the key in the file table
    void *raw_inode = gnl_ternary_search_tree_get(file_system->file_table, key);

    // if the key is not present return an error
    if (raw_inode == NULL) {
        gnl_logger_debug(file_system->logger, "Entry \"%s\" not found, returning with error", key);
        errno = EINVAL;

        return -1;
    }

    gnl_logger_debug(file_system->logger, "Entry \"%s\" found, removing", key);

    // else cast the raw_inode
    struct gnl_simfs_inode *inode = (struct gnl_simfs_inode *)raw_inode;

    // get the size of the inode
    int count = inode->size;

    // remove the file
    int res = gnl_ternary_search_tree_remove(file_system->file_table, key, destroy_file_table_inode);
    if (res == -1) {
        gnl_logger_debug(file_system->logger, "Remove on entry \"%s\" failed: %s", key, strerror(errno));

        //let the errno bubble

        return -1;
    }

    // update the file system
    file_system->heap_size -= count;

    gnl_logger_debug(file_system->logger, "Remove on entry \"%s\" succeeded, %d bytes freed, the heap size has now %d bytes left",
                     key, count, file_system->memory_limit - file_system->heap_size);

    return 0;
}

/**
 * Check if the file pointed by the given inode is lockable: if not, wait for it.
 * Attention! An invocation to this method may block the process until the file pointed
 * by the given inode returns lockable.
 *
 * @param file_system       The file system instance where the given inode resides.
 * @param inode             The inode pointing to the target file.
 *
 * @return                  Returns 0 on success, -1 otherwise.
 */
static int wait_file_to_be_lockable(struct gnl_simfs_file_system *file_system, struct gnl_simfs_inode *inode) {

    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(inode, EINVAL, -1)

    int test, res;
    while ((test = gnl_simfs_inode_has_hippie_pid(inode)) > 0) {
        GNL_MINUS1_CHECK(test, errno, -1)

        gnl_logger_debug(file_system->logger, "The file \"%s\" is opened (but not locked) by one or more pid, "
                                              "it can not be locked, waiting", inode->name);

        res = gnl_simfs_inode_wait_file_availability(inode, &(file_system->mtx));
        GNL_MINUS1_CHECK(res, errno, -1)
    }

    // if this point is reached, the target file is ready to be used

    gnl_logger_debug(file_system->logger, "The file \"%s\" is now lockable", inode->name);

    return 0;
}

/**
 * Return the inode referred by the given fd from the file descriptor table.
 *
 * @param file_system   The file system instance where the file table resides.
 * @param fd            The file descriptor.
 * @param pid           The current process id.
 *
 * @return              Returns the inode referred by fd on success,
 *                      NULL otherwise.
 */
static struct gnl_simfs_inode *get_inode_from_fd(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, NULL)

    // search the file in the file descriptor table
    struct gnl_simfs_inode *inode = gnl_simfs_file_descriptor_table_get(file_system->file_descriptor_table, fd, pid);

    // if the file is not present return an error
    if (inode == NULL) {
        gnl_logger_debug(file_system->logger, "File descriptor %d does not exist, returning with error", fd);

        //let the errno bubble

        return NULL;
    }

    gnl_logger_debug(file_system->logger, "File descriptor %d is pointing the file \"%s\"", fd, inode->name);

    return inode;
}

#include <gnl_macro_end.h>
