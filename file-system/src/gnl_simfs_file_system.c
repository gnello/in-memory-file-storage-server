#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <gnl_ternary_search_tree_t.h>
#include "./gnl_simfs_file_descriptor_table.c"
#include "./gnl_simfs_inode.c"
#include "../include/gnl_simfs_file_system.h"
#include <gnl_macro_beg.h>

// the maximum number of simultaneously open files allowed
#define GNL_SIMFS_MAX_OPEN_FILES 10240

/**
 * Macro to acquire the lock.
 */
#define GNL_SIMFS_LOCK_ACQUIRE(return_value) {                      \
    int lock_acquire_res = pthread_mutex_lock(&(file_system->mtx));   \
    GNL_MINUS1_CHECK(lock_acquire_res, errno, return_value)         \
}

/**
 * Macro to release the lock.
 */
#define GNL_SIMFS_LOCK_RELEASE(return_value) {                      \
int lock_release_res = pthread_mutex_unlock(&(file_system->mtx));     \
    GNL_MINUS1_CHECK(lock_release_res, errno, return_value)         \
}

/**
 * Macro to compare two values, if they are not equals
 * return an error and release the lock.
 */
#define GNL_SIMFS_COMPARE(actual, expected_error, errno_code, return_value) {   \
    if ((actual) == (expected_error)) {                                         \
        errno = errno_code;                                                     \
        GNL_SIMFS_LOCK_RELEASE(return_value)                                    \
                                                                                \
        return return_value;                                                    \
    }                                                                           \
}

/**
 * Macro to check that a return value is not NULL,
 * on error release the lock.
 */
#define GNL_SIMFS_NULL_CHECK(x, error_code, return_code) {  \
    GNL_SIMFS_COMPARE(x, NULL, error_code, return_code)     \
}

/**
 * Macro to check that a return value is not -1,
 * on error release the lock.
 */
#define GNL_SIMFS_MINUS1_CHECK(x, error_code, return_code) {    \
    GNL_SIMFS_COMPARE(x, -1, error_code, return_code)           \
}

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
    int heap_size;

    // the counter of the files present into the file system
    int files_count;

    // the number of files that can be handled by the file system
    int files_limit;

    // the memory allocable in megabyte by the file system
    int memory_limit;

    // contains all the open files in a precisely time,
    // the index is the file descriptor, the value is a
    // copy of the inode of the file.
    struct gnl_simfs_file_descriptor_table *file_descriptor_table;

    // the lock of the file system
    pthread_mutex_t mtx;
};

/**
 * {@inheritDoc}
 */
struct gnl_simfs_file_system *gnl_simfs_file_system_init(unsigned int memory_limit, unsigned int files_limit) {
    struct gnl_simfs_file_system *fs = (struct gnl_simfs_file_system *)malloc(sizeof(struct gnl_simfs_file_system));
    GNL_NULL_CHECK(fs, ENOMEM, NULL)

    // assign arguments
    fs->memory_limit = memory_limit;
    fs->files_limit = files_limit;

    // initialize the file table
    fs->file_table = NULL;

    // initialize the file descriptor table
    fs->file_descriptor_table = gnl_simfs_file_descriptor_table_init(GNL_SIMFS_MAX_OPEN_FILES);

    // initialize the other values
    fs->heap_size = 0;
    fs->files_count = 0;

    // initialize lock
    int res = pthread_mutex_init(&(fs->mtx), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    return fs;
}

/**
 * Destroy an inode. It should be passed to the gnl_ternary_search_tree_destroy
 * method of the gnl_ternary_search_tree data structure.
 *
 * @param ptr   The void pointer value of the gnl_ternary_search_tree data structure.
 */
static void destroy_inode(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    // implicitly cast the value
    struct gnl_simfs_inode *inode = ptr;

    // destroy the obtained inode
    gnl_simfs_inode_destroy(inode);
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_file_system_destroy(struct gnl_simfs_file_system *file_system) {
    if (file_system == NULL) {
        return;
    }

    // destroy the file table
    gnl_ternary_search_tree_destroy(&file_system->file_table, destroy_inode);

    // destroy the file descriptor table
    gnl_simfs_file_descriptor_table_destroy(file_system->file_descriptor_table);

    // destroy the lock, proceed on error
    pthread_mutex_destroy(&(file_system->mtx));

    // destroy the file system
    free(file_system);
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
static struct gnl_simfs_inode *create_file(struct gnl_simfs_file_system *file_system, char *filename) {
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
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init();
    GNL_NULL_CHECK(inode, errno, NULL)

    // put the inode into the file table
    int res = gnl_ternary_search_tree_put(&file_system->file_table, filename, inode);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // increment the files counter
    file_system->files_count++;

    return inode;
}

static int wait_file_unlock(struct gnl_simfs_file_system *file_system, struct gnl_simfs_inode *inode, unsigned int pid) {
    int is_file_locked = gnl_simfs_inode_is_file_locked(inode);
    GNL_SIMFS_MINUS1_CHECK(is_file_locked, errno, -1)

    return is_file_locked > 0 && is_file_locked != pid;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_system_open(struct gnl_simfs_file_system *file_system, char *filename, int flags, unsigned int pid) {
    // acquire the lock
    GNL_SIMFS_LOCK_ACQUIRE(-1)

    // validate the parameters
    GNL_SIMFS_NULL_CHECK(file_system, EINVAL, -1)

    // check if we can open a file
    if (gnl_simfs_file_descriptor_table_size(file_system->file_descriptor_table) == GNL_SIMFS_MAX_OPEN_FILES) {
        errno = ENFILE;
        GNL_SIMFS_LOCK_RELEASE(-1)

        return -1;
    }

    struct gnl_simfs_inode *inode;

    // if the file must be created
    if (GNL_SIMFS_O_CREATE & flags) {

        // search the file in the file table
        void *raw_inode = gnl_ternary_search_tree_get(file_system->file_table, filename);

        // if the file is present return an error
        GNL_SIMFS_MINUS1_CHECK(-1 * !(raw_inode == NULL), EEXIST, -1)

        // the file is not present, create it
        inode = create_file(file_system, filename);
        GNL_SIMFS_NULL_CHECK(inode, errno, -1)
    }
    // if the file must be present
    else {

        // search the file in the file table
        void *raw_inode = gnl_ternary_search_tree_get(file_system->file_table, filename);

        // if the file is not present return an error
        GNL_SIMFS_NULL_CHECK(raw_inode, ENOENT, -1)

        // else cast the raw_inode
        inode = (struct gnl_simfs_inode *)raw_inode;
    }

    // check if the file is locked: if the file is locked
    // and the owner is not the given pid, wait for it
    int test, res;
    while ((test = wait_file_unlock(file_system, inode, pid)) > 0) {
        GNL_SIMFS_MINUS1_CHECK(test, errno, -1)

        res = gnl_simfs_inode_wait_unlock(inode, &(file_system->mtx));
        GNL_SIMFS_MINUS1_CHECK(res, errno, -1)
    }

    // if this point is reached, the target file is unlocked
    // or is locked by the given pid

    // if the file must be locked, lock it
    if (GNL_SIMFS_O_LOCK & flags) {
        res = gnl_simfs_inode_file_lock(inode, pid);
        GNL_SIMFS_MINUS1_CHECK(res, errno, -1)
    }

    // increase the inode reference count
    res = gnl_simfs_inode_increase_refs(inode);
    GNL_SIMFS_MINUS1_CHECK(res, errno, -1)

    // put the inode into the file descriptor table; do not check an error here
    // because we are immediately returning anyway
    int fd = gnl_simfs_file_descriptor_table_put(file_system->file_descriptor_table, inode);

    // release the lock
    GNL_SIMFS_LOCK_RELEASE(-1)

    return fd;
}

#undef GNL_SIMFS_MAX_OPEN_FILES

#undef GNL_SIMFS_LOCK_ACQUIRE
#undef GNL_SIMFS_LOCK_RELEASE
#undef GNL_SIMFS_COMPARE
#undef GNL_SIMFS_NULL_CHECK
#undef GNL_SIMFS_MINUS1_CHECK

#include <gnl_macro_end.h>
