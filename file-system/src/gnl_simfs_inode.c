#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include "../include/gnl_simfs_inode.h"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
struct gnl_simfs_inode *gnl_simfs_inode_init(char *name) {
    struct gnl_simfs_inode *inode = (struct gnl_simfs_inode *)malloc(sizeof(struct gnl_simfs_inode));
    GNL_NULL_CHECK(inode, ENOMEM, NULL)

    // set the creation time
    inode->creation_time = time(NULL);

    // initialize condition variables
    int res = pthread_cond_init(&(inode->file_access_available), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // set the name
    GNL_CALLOC(inode->name, strlen(name) + 1, NULL)
    strncpy(inode->name, name, strlen(name));

    // initialize others attributes
    inode->last_modify_time = 0;
    inode->size = 0;
    inode->locked = 0;
    inode->direct_ptr = NULL;
    inode->active_hippie_pid = 0;
    inode->waiting_locker_pid = 0;

    return inode;
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_inode_destroy(struct gnl_simfs_inode *inode) {
    if (inode == NULL) {
        return;
    }

    // destroy the name
    free(inode->name);

    // destroy the file pointer
    free(inode->direct_ptr);

    // destroy the condition variables
    pthread_cond_destroy(&(inode->file_access_available));

    // destroy the inode
    free(inode);
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_is_file_locked(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    return inode->locked;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_wait_file_availability(struct gnl_simfs_inode *inode, pthread_mutex_t *mtx) {
    return pthread_cond_wait(&(inode->file_access_available), mtx);
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_increase_refs(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    inode->reference_count++;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_decrease_refs(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    if (inode->reference_count == 0) {
        errno = EPERM;
        return -1;
    }

    inode->reference_count--;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_file_lock(struct gnl_simfs_inode *inode, unsigned int pid) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // if the file is already locked by the
    // given pid return with success (idempotency)
    if (inode->locked == pid) {
        return 0;
    }

    // if the file is locked by a different
    // pid return an error
    if (inode->locked != 0) {
        errno = EPERM;
        return -1;
    }

    // lock the file
    inode->locked = pid;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_file_unlock(struct gnl_simfs_inode *inode, unsigned int pid) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    // if the file is already unlocked
    // return with success (idempotency)
    if (inode->locked == 0) {
        return 0;
    }

    // if the file is locked by a different
    // pid return an error
    if (inode->locked != pid) {
        errno = EPERM;
        return -1;
    }

    // unlock the file
    inode->locked = 0;

    // wake up eventually waiting threads
    return pthread_cond_signal(&(inode->file_access_available));
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_increase_hippie_pid(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    inode->active_hippie_pid++;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_decrease_hippie_pid(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    if (inode->active_hippie_pid == 0) {
        errno = EPERM;
        return -1;
    }

    inode->active_hippie_pid--;

    if (inode->active_hippie_pid == 0) {
        return pthread_cond_signal(&(inode->file_access_available));
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_has_hippie_pid(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    return inode->active_hippie_pid > 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_increase_locker_pid(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    inode->waiting_locker_pid++;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_decrease_locker_pid(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    if (inode->waiting_locker_pid == 0) {
        errno = EPERM;
        return -1;
    }

    inode->waiting_locker_pid--;

    if (inode->waiting_locker_pid == 0) {
        return pthread_cond_signal(&(inode->file_access_available));
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_has_locker_pid(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    return inode->waiting_locker_pid > 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_append_to_file(struct gnl_simfs_inode *inode, const void *buf, size_t count) {
    //validate the parameters
    GNL_NULL_CHECK(inode, EINVAL, -1)
    GNL_NULL_CHECK(buf, EINVAL, -1)

    // if we do not have to write data, return with an error
    GNL_MINUS1_CHECK(-1 * (count <= 0), EINVAL, -1)

    // calculate the new size
    int size = inode->size + count;

    // alloc the memory onto the heap for the write
    void *temp = realloc(inode->direct_ptr, size);

    // do not handle errors but bubble it, if an
    // update fails we are ok with the fact that
    // realloc does not free the original pointer
    GNL_NULL_CHECK(temp, errno, -1)

    // update the original pointer if it has changed (or not)
    inode->direct_ptr = temp;

    // write the data
    memcpy(inode->direct_ptr + inode->size, buf, count);

    // update the inode
    inode->size += count;
    inode->last_modify_time = time(NULL);

    return 0;
}

#include <gnl_macro_end.h>
