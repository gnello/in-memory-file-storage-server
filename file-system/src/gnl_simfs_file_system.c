#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <gnl_logger.h>
#include <gnl_huffman_tree.h>
#include "../include/gnl_simfs_file_system.h"
#include "./gnl_simfs_file_system_rts.c"
#include <gnl_macro_beg.h>

// the maximum number of simultaneously open files allowed
#define GNL_SIMFS_MAX_OPEN_FILES 10240

/**
 * Macro to acquire the lock.
 */
#define GNL_SIMFS_LOCK_ACQUIRE(return_value, pid) {                         \
    int lock_acquire_res = pthread_mutex_lock(&(file_system->mtx));         \
    GNL_MINUS1_CHECK(lock_acquire_res, errno, return_value)                 \
    gnl_logger_debug(file_system->logger, "Pid %d acquired the lock", pid); \
}

/**
 * Macro to release the lock.
 */
#define GNL_SIMFS_LOCK_RELEASE(return_value, pid) {                         \
    int lock_release_res = pthread_mutex_unlock(&(file_system->mtx));       \
    GNL_MINUS1_CHECK(lock_release_res, errno, return_value)                 \
    gnl_logger_debug(file_system->logger, "Pid %d released the lock", pid); \
}

/**
 * Macro to compare two values, if they are not equals
 * return an error and release the lock.
 */
#define GNL_SIMFS_COMPARE(actual, expected_error, errno_code, return_value, pid) {  \
    if ((actual) == (expected_error)) {                                             \
        errno = errno_code;                                                         \
        GNL_SIMFS_LOCK_RELEASE(return_value, pid)                                   \
                                                                                    \
        return return_value;                                                        \
    }                                                                               \
}

/**
 * Macro to check that a return value is not NULL,
 * on error release the lock.
 */
#define GNL_SIMFS_NULL_CHECK(x, error_code, return_code, pid) { \
    GNL_SIMFS_COMPARE(x, NULL, error_code, return_code, pid)    \
}

/**
 * Macro to check that a return value is not -1,
 * on error release the lock.
 */
#define GNL_SIMFS_MINUS1_CHECK(x, error_code, return_code, pid) {   \
    GNL_SIMFS_COMPARE(x, -1, error_code, return_code, pid)          \
}

/**
 * {@inheritDoc}
 */
struct gnl_simfs_file_system *gnl_simfs_file_system_init(unsigned int memory_limit, unsigned int files_limit,
        const char *log_path, const char *log_level, enum gnl_simfs_replacement_policy replacement_policy) {
    struct gnl_simfs_file_system *fs = (struct gnl_simfs_file_system *)malloc(sizeof(struct gnl_simfs_file_system));
    GNL_NULL_CHECK(fs, ENOMEM, NULL)

    // assign arguments, convert the memory limit from Megabytes to bytes
    fs->memory_limit = mb_to_bytes(memory_limit);
    fs->files_limit = files_limit;

    // initialize the file table
    fs->file_table = gnl_simfs_file_table_init();
    GNL_NULL_CHECK(fs->file_table, errno, NULL)

    // initialize the file descriptor table
    fs->file_descriptor_table = gnl_simfs_file_descriptor_table_init(GNL_SIMFS_MAX_OPEN_FILES);
    GNL_NULL_CHECK(fs->file_descriptor_table, errno, NULL)

    // initialize the replacement policy
    fs->replacement_policy = replacement_policy;

    // initialize the logger
    if (log_path == NULL) {
        // if no log_path is given do not create a logger
        fs->logger = NULL;
    } else {
        // if a log_path is given create a logger
        char *level = NULL;

        // if a log_level is given then use it, otherwise
        // set it to "error" on default
        if (log_level == NULL) {
            GNL_CALLOC(level, 6, NULL);
            strcpy(level, "error");
        } else {
            GNL_CALLOC(level, strlen(log_level) + 1, NULL);
            strncpy(level, log_level, strlen(log_level));
        }

        // finally, instantiate the logger
        fs->logger = gnl_logger_init(log_path, "gnl_simfs_file_system", level);

        // free the level
        free(level);

        // check the logger instantiation result
        GNL_NULL_CHECK(fs->logger, errno, NULL)
    }

    // initialize lock
    int res = pthread_mutex_init(&(fs->mtx), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // initialize the monitor
    fs->monitor = gnl_simfs_monitor_init();
    GNL_NULL_CHECK(fs->monitor, errno, NULL)

    gnl_logger_debug(fs->logger, "File system initialized. Memory limit: %f MB, max storable files: %d.",
                     bytes_to_mb(fs->memory_limit), fs->files_limit);

    return fs;
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_file_system_destroy(struct gnl_simfs_file_system *file_system) {
    if (file_system == NULL) {
        return;
    }

    gnl_logger_debug(file_system->logger, "Destroying the file system, all files will be lost.");

    // destroy the file descriptor table
    gnl_simfs_file_descriptor_table_destroy(file_system->file_descriptor_table);

    // destroy the file table
    gnl_simfs_file_table_destroy(file_system->file_table);

    // destroy the lock, proceed on error
    pthread_mutex_destroy(&(file_system->mtx));

    // destroy the monitor
    gnl_simfs_monitor_destroy(file_system->monitor);

    gnl_logger_debug(file_system->logger, "File system destroyed.");

    // destroy the logger
    gnl_logger_destroy(file_system->logger);

    // destroy the file system
    free(file_system);
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_open(struct gnl_simfs_file_system *file_system, const char *filename, int flags, unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)
    GNL_SIMFS_MINUS1_CHECK(-1 * (strlen(filename) == 0), EINVAL, -1, pid)

    int res;

    gnl_logger_debug(file_system->logger, "Open: pid %d is trying to open the file \"%s\"", pid, filename);

    if (GNL_SIMFS_O_CREATE & flags) {
        gnl_logger_debug(file_system->logger, "Open: O_CREATE flag set");
    }

    if (GNL_SIMFS_O_LOCK & flags) {
        gnl_logger_debug(file_system->logger, "Open: O_LOCK flag set");
    }

    // check if we can open a file
    if (gnl_simfs_file_descriptor_table_size(file_system->file_descriptor_table) == GNL_SIMFS_MAX_OPEN_FILES) {
        errno = ENFILE;

        gnl_logger_warn(file_system->logger, "Open failed: memory check to open file \"%s\" failed, "
                                             "max open files limit reached", filename);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // get the inode of the filename
    struct gnl_simfs_inode *inode = gnl_simfs_file_table_get(file_system->file_table, filename);

    // check getting error
    if (inode == NULL && errno != ENOENT) {
        gnl_logger_warn(file_system->logger, "Open failed: error on getting file \"%s\": %s", filename, strerror(errno));

        // let the errno bubble

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // if the file must be created
    if (GNL_SIMFS_O_CREATE & flags) {

        // if the file is present return an error
        if (inode != NULL) {
            gnl_logger_warn(file_system->logger, "Open failed: GNL_SIMFS_O_CREATE flag provided but file \"%s\" "
                                                  "already exists, returning with error", filename);
            errno = EEXIST;

            GNL_SIMFS_LOCK_RELEASE(-1, pid)

            return -1;
        }

        // the file is not present, create it
        inode = gnl_simfs_rts_create_inode(file_system, filename);
        GNL_SIMFS_NULL_CHECK(inode, errno, -1, pid)
    }
    // if the file must be present
    else {
        // if the file is not present return an error
        if (inode == NULL) {
            gnl_logger_warn(file_system->logger, "Open failed: GNL_SIMFS_O_CREATE flag not provided but file \"%s\" "
                                                  "does not exist, returning with error", filename);
            errno = ENOENT;

            GNL_SIMFS_LOCK_RELEASE(-1, pid)

            return -1;
        }
    }

    // get if the file is locked information
    int file_locked_by_pid = gnl_simfs_inode_is_file_locked(inode);
    GNL_SIMFS_MINUS1_CHECK(file_locked_by_pid, errno, -1, pid)

    // if the file is locked by any other pid return an error
    if (file_locked_by_pid > 0 && file_locked_by_pid != pid) {
        errno = EBUSY;

        gnl_logger_warn(file_system->logger, "Open failed: file \"%s\" is locked by pid %d and it can "
                                              "not be accessed", filename, file_locked_by_pid);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // if the file must be locked
    if (GNL_SIMFS_O_LOCK & flags) {

        // if the file is already locked by the given pid
        if (file_locked_by_pid == pid) {
            gnl_logger_debug(file_system->logger, "Open: file \"%s\" already locked by pid %d", filename, pid);
        } else {

            // lock the file
            res = gnl_simfs_rts_lock_inode(file_system, inode, pid);
            if (res == -1) {
                gnl_logger_warn(file_system->logger, "Open failed: file \"%s\" has a pending lock and it can not be "
                                                     "locked by pid %d, deadlock avoided", inode->name, pid);

                GNL_SIMFS_LOCK_RELEASE(-1, pid)

                // let the errno bubble
                return -1;
            }

            gnl_logger_debug(file_system->logger, "Open: file \"%s\" locked by pid %d", inode->name, pid);
        }
    }
    // else if we want to access the file without lock
    else {

        // if there are pending locks return an error
        int has_pending_locks = gnl_simfs_inode_has_pending_locks(inode);
        GNL_SIMFS_MINUS1_CHECK(has_pending_locks, errno, -1, pid)

        if (has_pending_locks > 0) {
            errno = EBUSY;

            gnl_logger_warn(file_system->logger, "Open failed: GNL_SIMFS_O_LOCK flag not provided but file \"%s\" "
                                                  "is waiting to be locked", filename);

            GNL_SIMFS_LOCK_RELEASE(-1, pid)

            return -1;
        }
    }

    // put the inode into the file descriptor table (the inode will be copied)
    int fd = gnl_simfs_file_descriptor_table_put(file_system->file_descriptor_table, inode, pid);
    GNL_SIMFS_MINUS1_CHECK(fd, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Open: created file descriptor %d for file %s", fd, filename);

    // increase the inode reference count
    res = gnl_simfs_inode_increase_refs(inode, pid);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Open: reference count of file %s increased, the file has now %d "
                                          "references", filename, inode->reference_count);

    gnl_logger_debug(file_system->logger, "Open: open on file \"%s\" succeeded, returning fd %d to pid %d", filename, fd, pid);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return fd;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_write(struct gnl_simfs_file_system *file_system, int fd, const void *buf, size_t count,
        unsigned int pid, struct gnl_list_t **evicted_list) {

    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)

    gnl_logger_debug(file_system->logger, "Write: pid %d is trying to write %d bytes in file descriptor %d", pid, count, fd);

    // compress the file to get the final size
    struct gnl_huffman_tree_artifact *artifact = gnl_huffman_tree_encode(buf, count);
    GNL_SIMFS_NULL_CHECK(artifact, errno, -1, pid)

    int final_count = gnl_huffman_tree_size(artifact);
    GNL_SIMFS_MINUS1_CHECK(final_count, errno, -1, pid)

    // destroy the artifact
    gnl_huffman_tree_destroy_artifact(artifact);

    gnl_logger_debug(file_system->logger, "Write: file pointed by file descriptor %d compressed", fd);
    gnl_logger_debug(file_system->logger, "Write: original size %d bytes", count);
    gnl_logger_debug(file_system->logger, "Write: final size %d bytes", final_count);

    // check if there is enough space to write the file
    if (final_count > file_system->memory_limit) {

        gnl_logger_warn(file_system->logger, "Write on file descriptor %d failed, the file is too big. "
                                             "Memory limit: %f MB, file size (compressed): %d bytes.",
                                             fd, bytes_to_mb(file_system->memory_limit), final_count);

        errno = E2BIG;
        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    int res;

    // check if there is enough space left to write the file
    int available_bytes;
    while (final_count > (available_bytes = gnl_simfs_rts_available_bytes(file_system))) {
        // check if there was an error on the gnl_simfs_rts_available_bytes invocation
        GNL_SIMFS_MINUS1_CHECK(available_bytes, errno, -1, pid);

        // if this point is reached, then there is no space left
        // into the file system to write final_count bytes

        // if there is no replacement policy, then fail (with honor)
        if (file_system->replacement_policy == GNL_SIMFS_RP_NONE) {
            // get the heap size
            int size = gnl_simfs_file_table_size(file_system->file_table);

            gnl_logger_warn(file_system->logger, "Write on file descriptor %d failed, max heap size reached."
                                                 "Memory limit: %f MB, current heap size: %lld bytes (%f MB), "
                                                 "prevented heap size overflowing by %d bytes", fd,
                                                 bytes_to_mb(file_system->memory_limit), size, bytes_to_mb(size),
                                                 final_count - available_bytes);

            errno = EDQUOT;
            GNL_SIMFS_LOCK_RELEASE(-1, pid)

            return -1;
        }

        gnl_logger_debug(file_system->logger, "No space available to write %d bytes, evicting some files", final_count);

        // else evict a file
        res = gnl_simfs_rts_evict(file_system, evicted_list);
        GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid);
    }

    // search the file in the file descriptor table
    struct gnl_simfs_inode *inode_copy = gnl_simfs_rts_get_inode_by_fd(file_system, fd, pid);
    GNL_SIMFS_NULL_CHECK(inode_copy, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Write: got file descriptor %d's inode", fd);

    // get if the file is locked information
    int file_locked_by_pid = gnl_simfs_inode_is_file_locked(inode_copy);
    GNL_SIMFS_MINUS1_CHECK(file_locked_by_pid, errno, -1, pid)

    // check if the file is not locked or if we own the lock
    if (file_locked_by_pid > 0 && file_locked_by_pid != pid) {
        errno = EBUSY;

        gnl_logger_warn(file_system->logger, "Write failed: file \"%s\" is locked by pid %d and it can not be "
                                              "accessed", inode_copy->name, file_locked_by_pid);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // write the given buf into the inode copy buffer
    int nwrite = gnl_simfs_inode_write(inode_copy, buf, count);
    GNL_SIMFS_MINUS1_CHECK(nwrite, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Write: %d bytes written into file descriptor %d's inode buffer", nwrite, fd);

    // update the inode into the file table, this invocation is
    // mandatory because we are working on a copy of the inode,
    // so the original one needs to be updated with the modified copy
    res = gnl_simfs_rts_fflush_inode(file_system, inode_copy);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Write: inode flushed, write on file descriptor %d succeeded", fd);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_read(struct gnl_simfs_file_system *file_system, int fd, void **buf, size_t *count, unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)

    gnl_logger_debug(file_system->logger, "Read: pid %d is trying to read from file descriptor %d", pid, fd);

    // search the file in the file descriptor table
    struct gnl_simfs_inode *inode_copy = gnl_simfs_rts_get_inode_by_fd(file_system, fd, pid);
    GNL_SIMFS_NULL_CHECK(inode_copy, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Read: got file descriptor %d's inode", fd);

    // get if the file is locked information
    int file_locked_by_pid = gnl_simfs_inode_is_file_locked(inode_copy);
    GNL_SIMFS_MINUS1_CHECK(file_locked_by_pid, errno, -1, pid)

    // check if the file is not locked or if we own the lock
    if (file_locked_by_pid > 0 && file_locked_by_pid != pid) {
        errno = EBUSY;

        gnl_logger_warn(file_system->logger, "Read failed: file \"%s\" is locked by pid %d and it can not be "
                                              "accessed", inode_copy->name, file_locked_by_pid);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // read the file into the given buf
    int res = gnl_simfs_rts_read_inode(file_system, inode_copy, buf, count);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Read: %d bytes read from file descriptor %d's inode", *count, fd);

    // update the inode into the file table, this invocation is
    // mandatory because we are working on a copy of the inode,
    // so the original one needs to be updated with the modified copy
    res = gnl_simfs_rts_fflush_inode(file_system, inode_copy);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Read: read on file descriptor %d succeeded, inode updated", fd);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_close(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)

    gnl_logger_debug(file_system->logger, "Close: pid %d is trying to close file descriptor %d", pid, fd);

    // search the file in the file descriptor table
    struct gnl_simfs_inode *inode_copy = gnl_simfs_rts_get_inode_by_fd(file_system, fd, pid);
    GNL_SIMFS_NULL_CHECK(inode_copy, errno, -1, pid)

    // copy the filename
    char *filename = calloc(sizeof (char), (strlen(inode_copy->name) + 1));
    GNL_SIMFS_NULL_CHECK(filename, ENOMEM, -1, pid)

    strncpy(filename, inode_copy->name, strlen(inode_copy->name));

    // remove the file descriptor from the file descriptor table
    int res = gnl_simfs_file_descriptor_table_remove(file_system->file_descriptor_table, fd, pid);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Close: file descriptor %d removed", fd);

    // search the key in the file table
    struct gnl_simfs_inode *inode = gnl_simfs_rts_get_inode(file_system, filename);

    // free memory
    free(filename);

    // check the returning value
    if (inode == NULL) {

        // if the file is not found it was surely deleted, ignore the
        // error and return success
        if (errno == ENOENT) {
            gnl_logger_debug(file_system->logger, "Close: close on file descriptor %d succeeded, "
                                                  "file descriptor %d destroyed, inode not found, it was probably "
                                                  "deleted", fd, fd);

            res = 0;
        }
        // else propagate the errno
        else {
            gnl_logger_error(file_system->logger, "Close failed: %s", strerror(errno));

            res = -1;
        }

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return res;
    }

    gnl_logger_debug(file_system->logger, "Close: entry \"%s\" found, closing file", inode->name);

    // get if the file is locked information
//    res = gnl_simfs_inode_is_file_locked(inode);
//    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    // remove lock if pid owns it
//    if (res > 0 && res == pid) {
//        res = gnl_simfs_inode_file_unlock(inode, pid);
//        GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)
//
//        gnl_logger_debug(file_system->logger, "Close: file \"%s\" unlocked by pid %d", inode->name, pid);
//    }

    // decrease the inode reference count
    res = gnl_simfs_inode_decrease_refs(inode, pid);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Close: reference count of file %s decreased, the file has now %d "
                                          "references", inode->name, inode->reference_count);

    gnl_logger_debug(file_system->logger, "Close: close on file descriptor %d succeeded, "
                                          "file descriptor %d destroyed, inode updated", fd, fd);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_remove(struct gnl_simfs_file_system *file_system, const char *filename, unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)
    GNL_SIMFS_MINUS1_CHECK(-1 * (strlen(filename) == 0), EINVAL, -1, pid)

    gnl_logger_debug(file_system->logger, "Remove: pid %d is trying to remove file %s", pid, filename);

    // search the file in the file table
    struct gnl_simfs_inode *inode = gnl_simfs_rts_get_inode(file_system, filename);
    GNL_SIMFS_NULL_CHECK(inode, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Remove: entry \"%s\" found, removing file", filename);

    // get if the file is locked information
    int file_locked_by_pid = gnl_simfs_inode_is_file_locked(inode);
    GNL_SIMFS_MINUS1_CHECK(file_locked_by_pid, errno, -1, pid)

    // check if the file is locked and if the given pid owns the lock
    if (file_locked_by_pid == 0 || file_locked_by_pid != pid) {
        errno = EPERM;

        gnl_logger_warn(file_system->logger, "Remove failed: file \"%s\" is not locked by pid %d", filename, pid);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // get the inode size for later logging
    int inode_size = inode->size;

    // remove the file
    int res = gnl_simfs_rts_remove_inode(file_system, filename);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    // get the table size for logging
    int size = gnl_simfs_file_table_size(file_system->file_table);

    gnl_logger_debug(file_system->logger, "Remove: remove of file \"%s\" succeeded, inode destoyed", filename);
    gnl_logger_debug(file_system->logger, "Remove: new heap size %ldd bytes (%f MB), %d bytes freed",
                     size, bytes_to_mb(size), inode_size);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_lock(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)

    gnl_logger_debug(file_system->logger, "Lock: pid %d is trying to lock file descriptor %d", pid, fd);

    // search the file in the file descriptor table
    struct gnl_simfs_inode *inode_copy = gnl_simfs_rts_get_inode_by_fd(file_system, fd, pid);
    GNL_SIMFS_NULL_CHECK(inode_copy, errno, -1, pid)

    // search the key in the file table
    struct gnl_simfs_inode *inode = gnl_simfs_rts_get_inode(file_system, inode_copy->name);
    GNL_SIMFS_NULL_CHECK(inode, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Lock: entry \"%s\" found, locking file", inode_copy->name);

    // get if the file is locked information
    int file_locked_by_pid = gnl_simfs_inode_is_file_locked(inode);
    GNL_SIMFS_MINUS1_CHECK(file_locked_by_pid, errno, -1, pid)

    // if the file is locked
    if (file_locked_by_pid > 0) {

        // if the file is locked by the given pid return with success
        if (file_locked_by_pid == pid) {
            gnl_logger_debug(file_system->logger, "Lock: file \"%s\" already locked by pid %d, returning", inode_copy->name, pid);

            GNL_SIMFS_LOCK_RELEASE(-1, pid)

            return 0;
        }
        // if the file is locked by any other pid return an error
        else {
            errno = EBUSY;

            gnl_logger_warn(file_system->logger, "Lock failed: file \"%s\" is locked by pid %d and it can "
                                                  "not be accessed", inode_copy->name, file_locked_by_pid);

            GNL_SIMFS_LOCK_RELEASE(-1, pid)

            return -1;
        }
    }

    // lock the inode
    int res = gnl_simfs_rts_lock_inode(file_system, inode, pid);
    if (res == -1) {
        gnl_logger_warn(file_system->logger, "Lock failed: file \"%s\" has a pending lock and it can not be "
                                             "locked by pid %d, deadlock avoided", inode->name, pid);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        // let the errno bubble
        return -1;
    }

    gnl_logger_debug(file_system->logger, "Lock: file \"%s\" locked by pid %d", inode_copy->name, pid);
    gnl_logger_debug(file_system->logger, "Lock: lock of file \"%s\" succeeded, inode updated", inode_copy->name);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_unlock(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)

    gnl_logger_debug(file_system->logger, "Unlock: pid %d is trying to unlock file descriptor %d", pid, fd);

    // search the file in the file descriptor table
    struct gnl_simfs_inode *inode_copy = gnl_simfs_rts_get_inode_by_fd(file_system, fd, pid);
    GNL_SIMFS_NULL_CHECK(inode_copy, errno, -1, pid)

    // search the key in the file table
    struct gnl_simfs_inode *inode = gnl_simfs_rts_get_inode(file_system, inode_copy->name);
    GNL_SIMFS_NULL_CHECK(inode, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Unlock: entry \"%s\" found, unlocking file", inode_copy->name);

    // get if the file is locked information
    int file_locked_by_pid = gnl_simfs_inode_is_file_locked(inode);
    GNL_SIMFS_MINUS1_CHECK(file_locked_by_pid, errno, -1, pid)

    // if the file is already unlocked return an error
    if (file_locked_by_pid == 0) {
        errno = EPERM;

        gnl_logger_warn(file_system->logger, "Unlock failed: file \"%s\" is already unlocked, it can not be "
                                              "unlocked further by pid %d", inode_copy->name, pid);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // if the file is locked by any other pid
    if (file_locked_by_pid != pid) {
        errno = EBUSY;

        gnl_logger_warn(file_system->logger, "Unlock failed: file \"%s\" is locked by pid %d and it can "
                                              "not be accessed", inode_copy->name, file_locked_by_pid);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // unlock the file
    int res = gnl_simfs_inode_file_unlock(inode, pid);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Unlock: file \"%s\" unlocked by pid %d", inode_copy->name, pid);

    gnl_logger_debug(file_system->logger, "Unlock: unlock of file \"%s\" succeeded, inode updated", inode_copy->name);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return 0;
}

/**
 * {@inheritDoc}
 */
struct gnl_list_t *gnl_simfs_file_system_ls(struct gnl_simfs_file_system *file_system, unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(NULL, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, NULL, pid)

    gnl_logger_debug(file_system->logger, "ls: pid %d is trying to list the files of the file system", pid);

    // get a list of all the files present into the file system
    errno = 0;
    struct gnl_list_t *res = gnl_simfs_file_table_list(file_system->file_table);

    // if an error occurred
    if (res == NULL && errno != 0) {
        gnl_logger_error(file_system->logger, "ls failed: %s", strerror(errno));

        GNL_SIMFS_LOCK_RELEASE(NULL, pid)

        return NULL;
    }

    gnl_logger_debug(file_system->logger, "ls: list of files succeeded");

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(NULL, pid)

    return res;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_stat(struct gnl_simfs_file_system *file_system, const char *filename,
                               struct gnl_simfs_inode *buf, unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)
    GNL_SIMFS_NULL_CHECK(filename, EINVAL, -1, pid)

    gnl_logger_debug(file_system->logger, "Stat: pid %d is trying to stat the file \"%s\"", pid, filename);

    // get the inode of the filename
    // search the file in the file table
    struct gnl_simfs_inode *inode = gnl_simfs_rts_get_inode(file_system, filename);

    // check getting error
    if (inode == NULL) {
        gnl_logger_warn(file_system->logger, "Stat failed: error on getting the inode of the file \"%s\": %s",
                         filename, strerror(errno));

        // let the errno bubble

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // copy the inode
    buf->btime = inode->btime;
    buf->mtime = inode->mtime;
    buf->atime = inode->atime;
    buf->ctime = inode->ctime;
    buf->size = inode->size;
    buf->reference_count = inode->reference_count;

    GNL_CALLOC(buf->name, strlen(inode->name) + 1, -1)
    strcpy(buf->name, inode->name);

    gnl_logger_debug(file_system->logger, "Stat: stat of file \"%s\" succeeded", filename);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_fstat(struct gnl_simfs_file_system *file_system, int fd, struct gnl_simfs_inode *buf,
        unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)

    // get the inode of the fd
    // search the file in the file descriptor table
    struct gnl_simfs_inode *inode = gnl_simfs_rts_get_inode_by_fd(file_system, fd, pid);
    GNL_SIMFS_NULL_CHECK(inode, errno, -1, pid)

    // copy the inode
    buf->btime = inode->btime;
    buf->mtime = inode->mtime;
    buf->atime = inode->atime;
    buf->ctime = inode->ctime;
    buf->size = inode->size;
    buf->reference_count = inode->reference_count;

    GNL_CALLOC(buf->name, strlen(inode->name) + 1, -1)
    strcpy(buf->name, inode->name);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_status(struct gnl_simfs_file_system *file_system) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)

    char *dest;
    int res = gnl_simfs_file_system_get_replacement_policy(file_system, &dest);
    GNL_MINUS1_CHECK(res, errno, -1);

    printf("\n");
    printf("File System status:\n");

    printf("Memory limit: %f MB (%lld bytes)\n", bytes_to_mb(file_system->memory_limit),
           file_system->memory_limit);
    printf("File limit: %d\n", file_system->files_limit);

    printf("Max files reached: %d\n", file_system->monitor->file_peak);
    printf("Max heap size reached: %f MB (%lld bytes)\n", bytes_to_mb(file_system->monitor->bytes_peak),
           file_system->monitor->bytes_peak);
    printf("Number of evictions (replacement policy: %s): %d\n", dest, file_system->monitor->file_evictions);

    // free memory
    free(dest);

    printf("Files stored at exit: %d\n", file_system->monitor->file_counter);
    printf("Heap size at exit: %f MB (%lld bytes)\n", bytes_to_mb(file_system->monitor->bytes_counter),
           file_system->monitor->bytes_counter);

    printf("File list:\n");

    // reset the errno
    errno = 0;

    struct gnl_list_t *list = gnl_simfs_file_table_list(file_system->file_table);

    // check the result
    if (list == NULL) {

        // if an error occurred
        if (errno != 0) {
            perror("error on getting the files");

            return -1;
        }

        printf("no files stored");
        printf("\n");

        return 0;
    }

    // print the file list
    struct gnl_list_t *current = list;
    while(current != NULL) {
        printf("%s\n", (char *)current->el);

        current = current->next;
    }

    printf("\n");

    gnl_list_destroy(&list, free);

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_get_replacement_policy(struct gnl_simfs_file_system *file_system, char **dest) {
    // validate the parameters
    GNL_NULL_CHECK(file_system, EINVAL, -1)

    switch (file_system->replacement_policy) {

        case GNL_SIMFS_RP_NONE:
        GNL_CALLOC(*dest, 5, -1);
            strcpy(*dest, "NONE");
            break;

        case GNL_SIMFS_RP_FIFO:
        GNL_CALLOC(*dest, 5, -1);
            strcpy(*dest, "FIFO");
            break;

        case GNL_SIMFS_RP_LIFO:
        GNL_CALLOC(*dest, 5, -1);
            strcpy(*dest, "LIFO");
            break;

        case GNL_SIMFS_RP_LRU:
        GNL_CALLOC(*dest, 4, -1);
            strcpy(*dest, "LRU");
            break;

        case GNL_SIMFS_RP_MRU:
        GNL_CALLOC(*dest, 4, -1);
            strcpy(*dest, "MRU");
            break;

        case GNL_SIMFS_RP_LFU:
        GNL_CALLOC(*dest, 4, -1);
            strcpy(*dest, "LFU");
            break;

        default:
            errno = EINVAL;
            return -1;
            /* UNREACHED */
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_remove_session(struct gnl_simfs_file_system *file_system, unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)

    gnl_logger_debug(file_system->logger, "Remove session: removing pid %d session from the file system", pid);

    // unlock the inodes locked by pid
    gnl_logger_debug(file_system->logger, "Remove session: unlocking every files locked by pid %d", pid);

    // get a list of files present into the file system
    struct gnl_list_t *list = gnl_simfs_file_table_list(file_system->file_table);
    GNL_SIMFS_NULL_CHECK(list, errno, -1, pid)

    struct gnl_list_t *current = list;

    int res;

    // scan the list
    while (current != NULL) {
        char *filename = (char *)current->el;

        // get the original inode of the filename
        struct gnl_simfs_inode *inode = gnl_simfs_rts_get_inode(file_system, filename);
        GNL_SIMFS_NULL_CHECK(inode, errno, -1, pid)

        // get the number of open file by pid
        int open_files = gnl_simfs_file_descriptor_table_pid_inode_size(file_system->file_descriptor_table, inode, pid);
        GNL_SIMFS_MINUS1_CHECK(open_files, errno, -1, pid)

        if (open_files > 0) {
            // decrease refs
            gnl_logger_debug(file_system->logger, "Remove session: decreasing refs of inode \"%s\" (%d refs)",
                             filename, open_files);

            while (open_files > 0) {
                res = gnl_simfs_inode_decrease_refs(inode, pid);
                GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

                open_files--;
            }
        }

        // if the given pid locked the inode, then unlock it
        if (gnl_simfs_inode_is_file_locked(inode) == pid) {
            res = gnl_simfs_inode_file_unlock(inode, pid);

            if (res == -1) {
                // let the errno bubble
                return -1;
            }

            gnl_logger_debug(file_system->logger, "Remove session: unlocked file \"%s\" previously locked by pid %d",
                             inode->name, pid);
        }

        current = current->next;
    }

    // remove the inodes of pid from the file descriptor table
    gnl_logger_debug(file_system->logger, "Remove session: removing pid %d open files", pid);

    res = gnl_simfs_file_descriptor_table_remove_pid(file_system->file_descriptor_table, pid);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    // free memory
    gnl_list_destroy(&list, free);

    gnl_logger_debug(file_system->logger, "Remove session: remove session of pid %d succeeded", pid);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return 0;
}

#undef GNL_SIMFS_MAX_OPEN_FILES

#undef GNL_SIMFS_LOCK_ACQUIRE
#undef GNL_SIMFS_LOCK_RELEASE
#undef GNL_SIMFS_COMPARE
#undef GNL_SIMFS_NULL_CHECK
#undef GNL_SIMFS_MINUS1_CHECK

#include <gnl_macro_end.h>
