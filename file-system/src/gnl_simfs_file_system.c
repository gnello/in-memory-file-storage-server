#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <gnl_logger.h>
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
int lock_release_res = pthread_mutex_unlock(&(file_system->mtx));           \
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
        const char *log_path, const char *log_level) {
    struct gnl_simfs_file_system *fs = (struct gnl_simfs_file_system *)malloc(sizeof(struct gnl_simfs_file_system));
    GNL_NULL_CHECK(fs, ENOMEM, NULL)

    // convert the memory limit from Megabytes to bytes
    int ml = memory_limit * 1048576;

    // assign arguments
    fs->memory_limit = ml;
    fs->files_limit = files_limit;

    // initialize the file table
    fs->file_table = NULL;

    // initialize the file descriptor table
    fs->file_descriptor_table = gnl_simfs_file_descriptor_table_init(GNL_SIMFS_MAX_OPEN_FILES);

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

    // initialize the other values
    fs->heap_size = 0;
    fs->files_count = 0;

    // initialize lock
    int res = pthread_mutex_init(&(fs->mtx), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    gnl_logger_debug(fs->logger, "File system initialized. Memory limit: %d bytes, max storable files: %d.", fs->memory_limit, fs->files_limit);

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
    gnl_ternary_search_tree_destroy(&file_system->file_table, destroy_file_table_inode);

    // destroy the lock, proceed on error
    pthread_mutex_destroy(&(file_system->mtx));

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

    gnl_logger_debug(file_system->logger, "Pid %d is trying to open the file \"%s\"", pid, filename);

    // check if we can open a file
    if (gnl_simfs_file_descriptor_table_size(file_system->file_descriptor_table) == GNL_SIMFS_MAX_OPEN_FILES) {
        errno = ENFILE;

        gnl_logger_warn(file_system->logger, "Open on file \"%s\" failed, max open files limit reached", filename);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    struct gnl_simfs_inode *inode;

    // if the file must be created
    if (GNL_SIMFS_O_CREATE & flags) {

        // search the file in the file table
        void *raw_inode = gnl_ternary_search_tree_get(file_system->file_table, filename);

        // if the file is present return an error
        if (raw_inode != NULL) {
            gnl_logger_debug(file_system->logger, "File \"%s\" already exists, returning with error", filename);
            errno = EEXIST;

            GNL_SIMFS_LOCK_RELEASE(-1, pid)

            return -1;
        }

        // the file is not present, create it
        inode = file_table_create(file_system, filename);
        GNL_SIMFS_NULL_CHECK(inode, errno, -1, pid)
    }
    // if the file must be present
    else {

        // search the file in the file table
        void *raw_inode = gnl_ternary_search_tree_get(file_system->file_table, filename);

        // if the file is not present return an error
        if (raw_inode == NULL) {
            gnl_logger_debug(file_system->logger, "File \"%s\" does not exist, returning with error", filename);
            errno = ENOENT;

            return -1;
        }

        // else cast the raw_inode
        inode = (struct gnl_simfs_inode *)raw_inode;
    }
    printf("GREPPAQUI size: %d - pointer: %p\n", inode->size, inode->direct_ptr);
    // get if the file is locked information
    int file_locked_by_pid = gnl_simfs_inode_is_file_locked(inode);
    GNL_SIMFS_MINUS1_CHECK(file_locked_by_pid, errno, -1, pid)

    // if the file must be locked
    if (GNL_SIMFS_O_LOCK & flags) {

        // if the file is locked by other pid return an error
        if (file_locked_by_pid > 0 && file_locked_by_pid != pid) {
            errno = EBUSY;

            gnl_logger_debug(file_system->logger, "Open failed: file \"%s\" locked by pid %d", filename, file_locked_by_pid);

            GNL_SIMFS_LOCK_RELEASE(-1, pid)

            return -1;
        }

        // increase the locker pid of the inode to inform that a lock
        // is pending
        res = gnl_simfs_inode_increase_locker_pid(inode);
        GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

        // check if the file can be locked: if not, wait for it
        res = wait_file_to_be_lockable(file_system, inode);
        GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

        // lock the file //TODO: prevent deadlock EDEADLK
        res = gnl_simfs_inode_file_lock(inode, pid);
        GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

        gnl_logger_debug(file_system->logger, "File \"%s\" locked by pid %d", filename, pid);

        // decrease the waiting locker pid
        res = gnl_simfs_inode_decrease_locker_pid(inode);
        GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)
    }
    // else if we want to access the file without lock
    else {

        // if the file is locked return an error
        if (file_locked_by_pid > 0) {
            errno = EBUSY;

            gnl_logger_debug(file_system->logger, "Open failed: file \"%s\" locked by pid %d", filename, file_locked_by_pid);

            GNL_SIMFS_LOCK_RELEASE(-1, pid)

            return -1;
        }

        // if there are pending locker pid return an error
        int has_locker_pid = gnl_simfs_inode_has_locker_pid(inode);
        GNL_SIMFS_MINUS1_CHECK(has_locker_pid, errno, -1, pid)

        if (has_locker_pid > 0) {
            errno = EBUSY;

            gnl_logger_debug(file_system->logger, "Open failed: file \"%s\" is waiting to be locked", filename);

            GNL_SIMFS_LOCK_RELEASE(-1, pid)

            return -1;
        }

        // increase the "hippie pid" count
        res = gnl_simfs_inode_increase_hippie_pid(inode);
        GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)
    }

    // increase the inode reference count
    res = gnl_simfs_inode_increase_refs(inode);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    // put the inode copy into the file descriptor table
    int fd = gnl_simfs_file_descriptor_table_put(file_system->file_descriptor_table, inode, pid);
    GNL_SIMFS_MINUS1_CHECK(fd, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Open on file \"%s\" succeeded, returning fd %d to pid %d", filename, fd, pid);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return fd;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_write(struct gnl_simfs_file_system *file_system, int fd, const void *buf, size_t count,
        unsigned int pid) {

    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1, pid)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1, pid)

    gnl_logger_debug(file_system->logger, "Pid %d is trying to write %d bytes in file descriptor %d", pid, count, fd);

    // check if there is enough space to write the file
    int available_bytes = file_system->memory_limit - file_system->heap_size;
    if (count > available_bytes) {

        gnl_logger_warn(file_system->logger, "Write on file descriptor %d failed, max heap size reached."
                                             "Memory limit: %d bytes, current heap size: %d bytes, "
                                             "prevented heap size overflowing by %d bytes", fd, file_system->memory_limit,
                                             file_system->heap_size, count - available_bytes);

        errno = E2BIG;
        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // search the file in the file descriptor table
    struct gnl_simfs_inode *inode_copy = get_inode_from_fd(file_system, fd, pid);
    GNL_SIMFS_NULL_CHECK(inode_copy, errno, -1, pid)

    // get if the file is locked information
    int file_locked_by_pid = gnl_simfs_inode_is_file_locked(inode_copy);
    GNL_SIMFS_MINUS1_CHECK(file_locked_by_pid, errno, -1, pid)

    // check if the file is not locked or if we own the lock
    if (file_locked_by_pid > 0 && file_locked_by_pid != pid) {
        errno = EBUSY;

        gnl_logger_debug(file_system->logger, "Write failed: file \"%s\" is locked by pid %d", inode_copy->name, pid);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    //TODO: da qui in poi in caso di errore non lasciare lo stato della struct corrotto

    // write the given buf into the file pointed by the inode
    int nwrite = gnl_simfs_inode_append_to_file(inode_copy, buf, count);
    GNL_SIMFS_MINUS1_CHECK(nwrite, errno, -1, pid)

    // update the inode into the file table, this invocation is
    // mandatory because we are working on a copy of the inode,
    // so the original one needs to be updated with the modified copy
    int res = file_table_fflush(file_system, inode_copy->name, inode_copy, nwrite);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Write on file descriptor %d succeeded, inode updated", fd);

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

    gnl_logger_debug(file_system->logger, "Pid %d is trying to close file descriptor %d", pid, fd);

    // search the file in the file descriptor table
    struct gnl_simfs_inode *inode_copy = get_inode_from_fd(file_system, fd, pid);
    GNL_SIMFS_NULL_CHECK(inode_copy, errno, -1, pid)

    // search the key in the file table
    void *raw_inode = gnl_ternary_search_tree_get(file_system->file_table, inode_copy->name);

    // if the key is not present return an error
    if (raw_inode == NULL) {
        gnl_logger_debug(file_system->logger, "Entry \"%s\" not found, returning with error", inode_copy->name);
        errno = EINVAL;

        return -1;
    }

    gnl_logger_debug(file_system->logger, "Entry \"%s\" found, closing file", inode_copy->name);

    // else cast the raw_inode
    struct gnl_simfs_inode *inode = (struct gnl_simfs_inode *)raw_inode;

    // get if the file is locked information
    int res = gnl_simfs_inode_is_file_locked(inode);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    // remove lock if pid owns it
    if (res > 0 && res == pid) {
        res = gnl_simfs_inode_file_unlock(inode, pid);
        GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

        gnl_logger_debug(file_system->logger, "File \"%s\" unlocked by pid %d", inode->name, pid);
    }
    // else decrease the hippie pid count
    else {
        res = gnl_simfs_inode_decrease_hippie_pid(inode); //TODO: rimuovere hippie count e fare con le reference
        GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)
    }

    // remove the file descriptor from the file descriptor table
    res = gnl_simfs_file_descriptor_table_remove(file_system->file_descriptor_table, fd, pid);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Close on file descriptor %d succeeded, "
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

    gnl_logger_debug(file_system->logger, "Pid %d is trying to remove file %s", pid, filename);

    // search the key in the file table
    void *raw_inode = gnl_ternary_search_tree_get(file_system->file_table, filename);

    // if the key is not present return an error
    if (raw_inode == NULL) {
        gnl_logger_debug(file_system->logger, "Entry \"%s\" not found, returning with error", filename);
        errno = EINVAL;

        return -1;
    }

    gnl_logger_debug(file_system->logger, "Entry \"%s\" found, removing file", filename);

    // else cast the raw_inode
    struct gnl_simfs_inode *inode = (struct gnl_simfs_inode *)raw_inode;

    // get if the file is locked information
    int file_locked_by_pid = gnl_simfs_inode_is_file_locked(inode);
    GNL_SIMFS_MINUS1_CHECK(file_locked_by_pid, errno, -1, pid)

    // check if the file is locked and if the given pid owns the lock
    if (file_locked_by_pid == 0 || file_locked_by_pid != pid) {
        errno = EPERM;

        gnl_logger_debug(file_system->logger, "Remove failed: file \"%s\" is not locked by pid %d", filename, pid);

        GNL_SIMFS_LOCK_RELEASE(-1, pid)

        return -1;
    }

    // remove the file
    int res = file_table_remove(file_system, filename);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1, pid)

    gnl_logger_debug(file_system->logger, "Remove of file \"%s\" succeeded, inode destoyed", filename);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1, pid)

    return 0;
}

// TODO: remove file, capire se si può fare (vedi reference count) (fare test per vedere cosa fa linux) -> vedere gnl_simfs_file_system_lock
// TODO: fare buffer s scrivere lì, riportare nell'inode solo alla chiusura (NO, meglio alla write) se il file non è stato rimosso
// TODO: la lettura può avvenire tranquillamente dalla copia buffer

#undef GNL_SIMFS_MAX_OPEN_FILES

#undef GNL_SIMFS_LOCK_ACQUIRE
#undef GNL_SIMFS_LOCK_RELEASE
#undef GNL_SIMFS_COMPARE
#undef GNL_SIMFS_NULL_CHECK
#undef GNL_SIMFS_MINUS1_CHECK

#include <gnl_macro_end.h>
