#include <errno.h>
#include <string.h>
#include <gnl_logger.h>
#include "../include/gnl_simfs_file_system.h"
#include "../include/gnl_simfs_file_system_struct.h"
#include "./gnl_simfs_file_table.c"
#include "./gnl_simfs_evicted_file.c"
#include "./gnl_simfs_monitor.c"
#include "./gnl_simfs_file_descriptor_table.c"
#include <gnl_macro_beg.h>

#define GNL_SIMFS_BYTES_IN_A_MEGABYTE 1048576

/**
 * Convert the given bytes into megabytes.
 *
 * @param bytes The bytes to convert.
 *
 * @return      Returns the megabytes value of
 *              the given bytes.
 */
static double bytes_to_mb(double bytes) {
    return bytes / GNL_SIMFS_BYTES_IN_A_MEGABYTE;
}

/**
 * Convert the given megabytes into bytes.
 *
 * @param mb    The megabytes to convert.
 *
 * @return      Returns the bytes value of
 *              the given megabytes.
 */
static double mb_to_bytes(unsigned long long mb) {
    return mb * GNL_SIMFS_BYTES_IN_A_MEGABYTE;
}

/**
 * Get the original inode of the given filename.
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

    // get the original inode of the filename
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
        gnl_logger_warn(file_system->logger, "Creation of file \"%s\" failed, max number of files reached (%d/%d)",
                         filename, count, file_system->files_limit);

        errno = EDQUOT;
        return NULL;
    }

    // check if there is enough memory
    int size = gnl_simfs_file_table_size(file_system->file_table);
    GNL_MINUS1_CHECK(size, errno, NULL)

    if (size == file_system->memory_limit) {
        gnl_logger_warn(file_system->logger, "Creation of file \"%s\" failed, max heap size reached (%d/%d)",
                         filename, size, file_system->memory_limit);

        errno = EDQUOT;
        return NULL;
    }

    // create a new inode
    struct gnl_simfs_inode *inode = gnl_simfs_file_table_create(file_system->file_table, filename);
    GNL_NULL_CHECK(inode, errno, NULL)

    gnl_logger_debug(file_system->logger, "Created file \"%s\"", filename);

    // log the new file table count
    count = gnl_simfs_file_table_count(file_system->file_table);
    GNL_MINUS1_CHECK(count, errno, NULL);

    gnl_logger_debug(file_system->logger, "The file system has now %d files", count);

    // track the event
    int res = gnl_simfs_monitor_file_added(file_system->monitor);
    GNL_MINUS1_CHECK(res, errno, NULL);

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

    // get the current inode size to calculate
    // the bytes that will be added
    int old_size = inode->size;

    // update the inode with the new entry
    int res = gnl_simfs_file_table_fflush(file_system->file_table, inode);
    if (res == -1) {
        gnl_logger_warn(file_system->logger, "File flush on entry \"%s\" failed: %s", inode->name, strerror(errno));

        //let the errno bubble

        return -1;
    }

    // track the event calculating the bytes added
    res = gnl_simfs_monitor_bytes_added(file_system->monitor, inode->size - old_size);
    GNL_MINUS1_CHECK(res, errno, -1);

    // log the new file table size
    int size = gnl_simfs_file_table_size(file_system->file_table);
    GNL_MINUS1_CHECK(size, errno, -1);

    gnl_logger_debug(file_system->logger, "File flush on entry \"%s\" succeeded", inode->name);
    gnl_logger_debug(file_system->logger, "Inode compressed into %d bytes", inode->size);
    gnl_logger_debug(file_system->logger, "The heap size is now %f MB (%lld bytes)", bytes_to_mb(size), size);

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
        gnl_logger_warn(file_system->logger, "Remove on entry \"%s\" failed: %s", key, strerror(errno));

        //let the errno bubble

        return -1;
    }

    // track the event
    res = gnl_simfs_monitor_bytes_removed(file_system->monitor, count);
    GNL_MINUS1_CHECK(res, errno, -1);

    res = gnl_simfs_monitor_file_removed(file_system->monitor);
    GNL_MINUS1_CHECK(res, errno, -1);

    // get the new file table size for logging
    int size = gnl_simfs_file_table_size(file_system->file_table);
    GNL_MINUS1_CHECK(size, errno, -1);

    gnl_logger_debug(file_system->logger, "Remove on entry \"%s\" succeeded, %d bytes freed", key, count);
    gnl_logger_debug(file_system->logger, "The heap size is now %f MB (%lld bytes)", bytes_to_mb(size), size);
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
 * Lock the file pointed by the given inode,
 *
 * @param file_system   The file system instance where the given inode resides.
 * @param inode         The inode pointing to the file to lock.
 * @param pid           The current process id.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int gnl_simfs_rts_lock_inode(struct gnl_simfs_file_system *file_system, struct gnl_simfs_inode *inode, int pid) {

    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // check if there are pending locks, since the intention
    // here is to lock the inode, if the inode is not
    // locked and there are pending locks, then this check prevents
    // the occurring of a deadlock
    int res = gnl_simfs_inode_has_pending_locks(inode);
    GNL_MINUS1_CHECK(-1 * res, EDEADLK, -1)

    // increase the pending locks count of the inode
    // to inform that a lock is pending
    res = gnl_simfs_inode_increase_pending_locks(inode);
    GNL_MINUS1_CHECK(res, errno, -1)

    // check if the file can be locked: if not, wait for it
    res = gnl_simfs_rts_wait_file_to_be_lockable(file_system, inode, pid);
    GNL_MINUS1_CHECK(res, errno, -1)

    // lock the file
    res = gnl_simfs_inode_file_lock(inode, pid);
    GNL_MINUS1_CHECK(res, errno, -1)

    // decrease the pending_locks count
    res = gnl_simfs_inode_decrease_pending_locks(inode);
    GNL_MINUS1_CHECK(res, errno, -1)

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

/**
 * Get the available bytes left on the heap for the given file system.
 *
 * @param file_system   The file system instance where to get the available
 *                      bytes.
 *
 * @return              The number of available bytes in the file system
 *                      on success, -1 otherwise.
 */
static int gnl_simfs_rts_available_bytes(struct gnl_simfs_file_system *file_system) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)

    int size = gnl_simfs_file_table_size(file_system->file_table);
    GNL_MINUS1_CHECK(size, errno, -1);

    return file_system->memory_limit - size;
}

/**
 * Build a min heap of victims in accordance with the given replacement policy.
 *
 * @param file_system   The file system instance to use to build a min heap of victims.
 * @param list          The list of filename present into the file system.
 *
 * @return              Return the min heap of victims created on success,
 *                      NULL otherwise.
 */
static struct gnl_min_heap_t *gnl_simfs_rts_build_victim_heap(struct gnl_simfs_file_system *file_system, struct gnl_list_t *list) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, NULL)

    gnl_logger_debug(file_system->logger, "Building the min heap of victims");

    struct gnl_list_t *current = list;

    // initialize the min heap
    struct gnl_min_heap_t *min_heap = gnl_min_heap_init();
    GNL_NULL_CHECK(min_heap, errno, NULL)

    int res;

    // scan the list
    while (current != NULL) {
        char *filename = (char *)current->el;

        // get the original inode of the filename
        struct gnl_simfs_inode *inode = gnl_simfs_rts_get_inode(file_system, filename);
        GNL_NULL_CHECK(inode, errno, NULL)

        int key;

        // get the right key in accordance with the replacement policy
        switch (file_system->replacement_policy) {
            case GNL_SIMFS_RP_FIFO:
                key = inode->btime;
                break;
            case GNL_SIMFS_RP_LIFO:
                key = (-1 * inode->btime);
                break;
            case GNL_SIMFS_RP_LRU:
                key = inode->ctime;
                break;
            case GNL_SIMFS_RP_MRU:
                key = (-1 * inode->ctime);
                break;
            case GNL_SIMFS_RP_LFU:
                key = inode->reference_count;
                break;
            default:
                errno = EINVAL;
                return NULL;
                /* UNREACHED */
        }

        // insert the element into the min heap
        res = gnl_min_heap_insert(min_heap, inode, key);
        GNL_MINUS1_CHECK(res, errno, NULL)

        current = current->next;
    }

    gnl_logger_debug(file_system->logger, "Min heap of victims built");

    return min_heap;
}

/**
 * Evict a file from the file system within the given replacement policy struct.
 *
 * @param file_system   The file system instance to use to evict.
 * @param buf           The buffer pointer where to write the evicted data.
 * @param count         The count of bytes evicted.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
int gnl_simfs_rts_evict(struct gnl_simfs_file_system *file_system, struct gnl_list_t **evicted_list) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(evicted_list, EINVAL, -1)

    gnl_logger_debug(file_system->logger, "Start eviction");

    // track the event
    int res = gnl_simfs_monitor_eviction_started(file_system->monitor);
    GNL_MINUS1_CHECK(res, errno, -1);

    gnl_logger_debug(file_system->logger, "Listing all the files in the filesystem");

    // get a list of files present into the file system
    struct gnl_list_t *list = gnl_simfs_file_table_list(file_system->file_table);
    GNL_NULL_CHECK(list, errno, -1)

    // build the replacement_policy
    struct gnl_min_heap_t *min_heap = gnl_simfs_rts_build_victim_heap(file_system, list);
    GNL_NULL_CHECK(min_heap, errno, -1)

    // free memory
    gnl_list_destroy(&list, free);

    // get the victim inode
    struct gnl_simfs_inode *victim_inode = gnl_min_heap_extract_min(min_heap);
    GNL_NULL_CHECK(victim_inode, errno, -1)

    gnl_logger_debug(file_system->logger, "Victim selected: \"%s\", %d bytes", victim_inode->name, victim_inode->size);

    // free memory
    gnl_min_heap_destroy(min_heap, NULL);

    // create an evicted file element
    struct gnl_simfs_evicted_file *evicted_file = gnl_simfs_evicted_file_init();
    GNL_NULL_CHECK(evicted_file, errno, -1)

    GNL_CALLOC(evicted_file->name, strlen(victim_inode->name) + 1, -1)
    strncpy(evicted_file->name, victim_inode->name, strlen(victim_inode->name));

    // read the file into the evicted element
    res = gnl_simfs_rts_read_inode(file_system, victim_inode, &(evicted_file->bytes), &(evicted_file->count));
    GNL_MINUS1_CHECK(res, errno, -1)

    // add the evicted file into the list
    res = gnl_list_insert(evicted_list, evicted_file);
    GNL_MINUS1_CHECK(res, errno, -1)

    gnl_logger_debug(file_system->logger, "Victim inserted into the evicted list");

    // remove the file
    res = gnl_simfs_rts_remove_inode(file_system, evicted_file->name);
    GNL_MINUS1_CHECK(res, errno, -1)

    gnl_logger_debug(file_system->logger, "Victim destroyed");
    gnl_logger_debug(file_system->logger, "Eviction ended with success");

    return 0;
}

#undef GNL_SIMFS_BYTES_IN_A_MEGABYTE

#include <gnl_macro_end.h>
