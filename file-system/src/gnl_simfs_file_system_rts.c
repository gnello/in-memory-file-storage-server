#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <gnl_logger.h>
#include "./gnl_simfs_file_table.c"
#include "./gnl_simfs_file_descriptor_table.c"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
struct gnl_simfs_file_system {

    // the file system file table, contains all the inodes of the
    // files present into the file system
    struct gnl_simfs_file_table *file_table;

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
 * Get the inode of the given filename.
 *
 * @param file_system   The file system instance where the file table resides.
 * @param filename      The filename of the file pointed by the inode to get.
 *
 * @return              Returns the inode of the given filename on success,
 *                      NULL otherwise.
 */
static struct gnl_simfs_inode *gnl_simfs_rts_get_inode(struct gnl_simfs_file_system *file_system, const char *filename) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, NULL)
    GNL_MINUS1_CHECK(-1 * (strlen(filename) == 0), EINVAL, NULL)

    // get the inode of the filename
    struct gnl_simfs_inode *inode = gnl_simfs_file_table_get(file_system->file_table, filename);

    // check getting error
    if (inode == NULL) {
        if (errno != ENOENT) {
            gnl_logger_debug(file_system->logger, "Error on getting file \"%s\": %s", filename, strerror(errno));
        } else {
            gnl_logger_debug(file_system->logger, "Entry \"%s\" not found, returning", filename);
        }

        // let the errno bubble

        return NULL;
    }

    return inode;
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
static struct gnl_simfs_inode *gnl_simfs_rts_create_inode(struct gnl_simfs_file_system *file_system, const char *filename) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, NULL)
    GNL_MINUS1_CHECK(-1 * (strlen(filename) == 0), EINVAL, NULL)

    gnl_logger_debug(file_system->logger, "Creating file: \"%s\"", filename);

    // check if we can create a new file
    int count = gnl_simfs_file_table_count(file_system->file_table);
    GNL_MINUS1_CHECK(count, errno, NULL)

    if (count == file_system->files_limit) {
        gnl_logger_debug(file_system->logger, "Creation of file \"%s\" failed, max number of files reached (%d/%d)",
                         filename, count, file_system->files_limit);

        errno = EDQUOT;
        return NULL;
    }

    // check if there is enough memory
    int size = gnl_simfs_file_table_size(file_system->file_table);
    GNL_MINUS1_CHECK(size, errno, NULL)

    if (size == file_system->memory_limit) {
        gnl_logger_debug(file_system->logger, "Creation of file \"%s\" failed, max heap size reached (%d/%d)",
                         filename, size, file_system->memory_limit);

        errno = EDQUOT;
        return NULL;
    }

    // create a new inode
    struct gnl_simfs_inode *inode = gnl_simfs_file_table_create(file_system->file_table, filename);
    GNL_NULL_CHECK(inode, errno, NULL)

    gnl_logger_debug(file_system->logger, "Created file \"%s\"", filename);

    return inode;
}

/**
 * Update an existing file table entry with the given, more updated, inode.
 *
 * @param file_system   The file system instance where the file table resides.
 * @param buffer_entry  The inode to use to fflush.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int gnl_simfs_rts_fflush_inode(struct gnl_simfs_file_system *file_system, struct gnl_simfs_inode *inode) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(inode, EINVAL, -1)

    gnl_logger_debug(file_system->logger, "Flushing inode of file entry \"%s\" into the file table", inode->name);

    // update the inode with the new entry
    int res = gnl_simfs_file_table_fflush(file_system->file_table, inode);
    if (res == -1) {
        gnl_logger_debug(file_system->logger, "File flush on entry \"%s\" failed: %s", inode->name, strerror(errno));

        //let the errno bubble

        return -1;
    }

    // log the new file table size
    int size = gnl_simfs_file_table_size(file_system->file_table);
    GNL_MINUS1_CHECK(size, errno, -1);

    gnl_logger_debug(file_system->logger, "File flush on entry \"%s\" succeeded", inode->name);
    gnl_logger_debug(file_system->logger, "Inode compressed into %d bytes", inode->size);
    gnl_logger_debug(file_system->logger, "The heap size has now %d bytes left", file_system->memory_limit - size);

    return 0;
}

/**
 * Read the file within the original node of the given inode_copy.
 *
 * @param file_system   The file system instance where the file table resides.
 * @param inode_copy    The copy of the inode to read.
 * @param buf           The buffer pointer where to write the read data.
 * @param count         The count of bytes read.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int gnl_simfs_rts_read_inode(struct gnl_simfs_file_system *file_system, struct gnl_simfs_inode *inode_copy,
        void **buf, size_t *count) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(inode_copy, EINVAL, -1)

    gnl_logger_debug(file_system->logger, "Reading inode of file entry \"%s\" from the file table", inode_copy->name);

    // search the key in the file table
    struct gnl_simfs_inode *inode = gnl_simfs_file_table_get(file_system->file_table, inode_copy->name);
//TODO: se dopo si fa il fflush non è necessario leggere l'inode originale
    // if the key is not present return an error
    GNL_NULL_CHECK(inode, errno, -1)

    // read the file into the given buf
    int res = gnl_simfs_inode_read(inode, buf, count);
    GNL_MINUS1_CHECK(res, errno, -1);

    gnl_logger_debug(file_system->logger, "Read on entry \"%s\" succeeded", inode_copy->name);

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
static int gnl_simfs_rts_remove_inode(struct gnl_simfs_file_system *file_system, const char *key) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_MINUS1_CHECK(-1 * (strlen(key) == 0), EINVAL, -1)

    gnl_logger_debug(file_system->logger, "Removing entry \"%s\" from the file table", key);

    // search the file in the file table
    struct gnl_simfs_inode *inode = gnl_simfs_rts_get_inode(file_system, key);
    GNL_NULL_CHECK(inode, errno, -1)

    gnl_logger_debug(file_system->logger, "Entry \"%s\" found, removing", key);

    // get the size of the inode
    int count = inode->size;

    // remove the file
    int res = gnl_simfs_file_table_remove(file_system->file_table, key);
    if (res == -1) {
        gnl_logger_debug(file_system->logger, "Remove on entry \"%s\" failed: %s", key, strerror(errno));

        //let the errno bubble

        return -1;
    }

    int size = gnl_simfs_file_table_size(file_system->file_table);
    GNL_MINUS1_CHECK(size, errno, -1);

    gnl_logger_debug(file_system->logger, "Remove on entry \"%s\" succeeded, %d bytes freed, the heap size has now %d bytes left",
                     key, count, file_system->memory_limit - size);

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
static int gnl_simfs_rts_wait_file_to_be_lockable(struct gnl_simfs_file_system *file_system, struct gnl_simfs_inode *inode, int pid) {

    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // get if the file is locked information
    int file_locked_by_pid = gnl_simfs_inode_is_file_locked(inode);
    GNL_MINUS1_CHECK(file_locked_by_pid, errno, -1)

    // if the file is locked return an error
    if (file_locked_by_pid > 0) {
        errno = EPERM;

        return -1;
    }

    int test, res;
    while ((test = (gnl_simfs_inode_has_refs(inode) && gnl_simfs_inode_has_other_pid_refs(inode, pid) == 1)) > 0) {
        GNL_MINUS1_CHECK(test, errno, -1)

        gnl_logger_debug(file_system->logger, "The file \"%s\" is opened (but not locked) by one or more pid, "
                                              "it can not be locked, waiting", inode->name);

        res = gnl_simfs_inode_wait_file_lockability(inode, &(file_system->mtx));
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
static struct gnl_simfs_inode *gnl_simfs_rts_get_inode_by_fd(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid) {
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
