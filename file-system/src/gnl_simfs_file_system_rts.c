#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <gnl_ternary_search_tree_t.h>
#include <gnl_logger.h>
#include "./gnl_simfs_file_descriptor_table.c"
#include "./gnl_simfs_inode.c"
#include "../include/gnl_simfs_file_system.h"
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

    // the memory allocable in megabyte by the file system
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
 * Create a new file and put it into the given file system.
 *
 * @param file_system   The file system instance where to put the created file.
 * @param filename      The filename of the file to create.
 *
 * @return              Returns the inode of the created file on success,
 *                      NULL otherwise.
 */
static struct gnl_simfs_inode *create_file(struct gnl_simfs_file_system *file_system, const char *filename) {

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
 * Check whether we should wait for a file available signal.
 *
 * We should wait in three cases:
 * - case 1:        the file is locked and the given pid does not own the lock
 * - case 2:        the file is not locked but there are "active hippie pid"
 * - case 3:        there are one or more waiting locker pid and the given pid
 *                  is a hippie pid
 *
 * There is an invalid case that causes an error if it happens:
 * - invalid case:  the file is locked but there are "active hippie pid".
 * The invalid case is invalid because breaks the Safety property "Never a file
 * can be locked and have happy pid in the same time".
 *
 * @param inode             The inode where to check.
 * @param pid               The current process id.
 * @param lock_requested    Whether or not the given pid has requested to
 *                          lock the file; 1 means yes, 0 means no.
 *
 * @return                  Returns 1 if we should wait, 0 if not,
 *                          -1 on error.
*/ //TODO: aggiungere test appena possibile (mancano gli altri metodi)
static int is_file_not_available(struct gnl_simfs_inode *inode, unsigned int pid, int lock_requested) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    int is_file_locked = gnl_simfs_inode_is_file_locked(inode);
    GNL_MINUS1_CHECK(is_file_locked, errno, -1)

    int has_hippie_pid = gnl_simfs_inode_has_hippie_pid(inode);
    GNL_MINUS1_CHECK(has_hippie_pid, errno, -1)

    int has_locker_pid = gnl_simfs_inode_has_locker_pid(inode);
    GNL_MINUS1_CHECK(has_locker_pid, errno, -1)

    // check the Safety property: invalid case
    if (is_file_locked > 0 && has_hippie_pid) {
        errno = ENOTRECOVERABLE;
        return -1;
    }

    return (is_file_locked > 0 && is_file_locked != pid) //case 1
    || (lock_requested > 0 && has_hippie_pid) // case 2
    || (lock_requested == 0 && has_locker_pid); // case 3
}

/**
 * Update an existing file table entry with the given new entry.
 *
 * @param file_system   The file system instance where the file table resides.
 * @param key           The key of the entry to be updated.
 * @param new_entry     The new entry to use to update the existing entry.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int update_file_table_entry(struct gnl_simfs_file_system *file_system, const char *key, const struct gnl_simfs_inode *new_entry) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(key, EINVAL, -1)
    GNL_NULL_CHECK(new_entry, EINVAL, -1)

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
    //TODO: non è detto che ci sia stata una write, prendere l'inode vecchio e confrontare la size con quello nuovo
    int res = gnl_simfs_inode_update(inode, new_entry); //TODO: capire cosa fare con copie di inode modificati in tempi diversi
    if (res == -1) {
        gnl_logger_debug(file_system->logger, "Update on entry \"%s\" failed: %s", key, strerror(errno));

        //let the errno bubble

        return -1;
    }

    // update the file system
    file_system->heap_size += inode->size; //TODO: sistemare con la size corretta (da confrontare con l'inode vecchio)

    gnl_logger_debug(file_system->logger, "Update on entry \"%s\" succeeded, the heap size is now %d bytes",
                     key, file_system->heap_size);

    return 0;
}

/**
 * Check if the file pointed by the given inode is available: if not, wait for it.
 * Attention! An invocation to this method may block the process until the file pointed
 * by the given inode returns available.
 *
 * @param file_system       The file system instance containing the lock to be released
 *                          in case of waiting.
 * @param inode             The inode pointing to the target file.
 * @param pid               The current process id.
 * @param lock_requested    Whether or not the given pid has requested to
 *                          lock the file; 1 means yes, 0 means no.
 *
 * @return                  Returns 0 on success, -1 otherwise.
 */
static int wait_file_availability(struct gnl_simfs_file_system *file_system, struct gnl_simfs_inode *inode,
        unsigned int pid, int lock_requested) {

    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)
    GNL_NULL_CHECK(inode, EINVAL, -1)

    int test, res;
    while ((test = is_file_not_available(inode, pid, lock_requested)) > 0) {
        GNL_MINUS1_CHECK(test, errno, -1)

        gnl_logger_debug(file_system->logger, "The file \"%s\" is not available, waiting", inode->name);

        res = gnl_simfs_inode_wait_file_availability(inode, &(file_system->mtx));
        GNL_MINUS1_CHECK(res, errno, -1)
    }

    // if this point is reached, the target file is ready to be used

    gnl_logger_debug(file_system->logger, "The file \"%s\" is now available", inode->name);

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
