#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include "../include/gnl_simfs_inode.h"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
struct gnl_simfs_inode *gnl_simfs_inode_init(const char *name) {
    struct gnl_simfs_inode *inode = (struct gnl_simfs_inode *)malloc(sizeof(struct gnl_simfs_inode));
    GNL_NULL_CHECK(inode, ENOMEM, NULL)

    // set the creation time of the file
    inode->btime = time(NULL);

    // initialize condition variables
    int res = pthread_cond_init(&(inode->file_access_available), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // set the name
    GNL_CALLOC(inode->name, strlen(name) + 1, NULL)
    strncpy(inode->name, name, strlen(name));

    // initialize others attributes
    inode->mtime = 0;
    inode->atime = 0;
    inode->size = 0;
    inode->locked = 0;
    inode->direct_ptr = NULL;
    inode->active_hippie_pid = 0;
    inode->waiting_locker_pid = 0;
    inode->reference_count = 0;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    return inode;
}

/**
 * Destroy the given inode. This method it supposed to be called with with_pointed_file=1
 * when the intention is to destroy the inode, with with_pointed_file=0 if the intention
 * is to destroy an its copy. This difference it is necessary because the copy of the inode
 * contains a direct pointer to the original file within the inode from which the copy was
 * made, so prevents accidental deletions.
 *
 * @param inode             The inode instance to destroy.
 * @param with_pointed_file If > 0, an invocation to this method will delete
 *                          also the file pointed by the given inode, If <= 0
 *                          the pointed file will be preserved.
 */
static void destroy_inode(struct gnl_simfs_inode *inode, int with_pointed_file) {
    if (inode == NULL) {
        return;
    }

    // destroy the name
    free(inode->name);

    // destroy the file pointer
    if (with_pointed_file > 0) {
        free(inode->direct_ptr);
        inode->direct_ptr = NULL;
    }

    // clear the waiting queue
//    pthread_cond_broadcast(&(inode->file_access_available));

    // destroy the condition variables
    pthread_cond_destroy(&(inode->file_access_available));

    // destroy the inode
    free(inode);
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_inode_destroy(struct gnl_simfs_inode *inode) {
    destroy_inode(inode, 1);
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_inode_copy_destroy(struct gnl_simfs_inode *inode) {
    destroy_inode(inode, 0);
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

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

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

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

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

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

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

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    // wake up eventually waiting threads
    return pthread_cond_signal(&(inode->file_access_available));
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_increase_hippie_pid(struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, -1)

    inode->active_hippie_pid++;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

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

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

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

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

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

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

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

    // alloc the memory onto the heap for the writing
    void *temp = realloc(inode->direct_ptr, size);

    // do not handle errors but bubble it, if an
    // update fails we are ok with the fact that
    // realloc does not free the original pointer
    GNL_NULL_CHECK(temp, errno, -1)

    // update the original pointer if it has changed (or not)
    inode->direct_ptr = temp;

    // write the data
    memcpy((char *)inode->direct_ptr + inode->size, buf, count);

    // update the size of the file within the inode
    inode->size += count;

    // update the last modification timestamp of the file
    // within the inode
    inode->mtime = time(NULL);

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    return 0;
}

/**
 * {@inheritDoc}
 */
struct gnl_simfs_inode *gnl_simfs_inode_copy(const struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(inode, EINVAL, NULL)

    // allocate space for the new inode
    struct gnl_simfs_inode *inode_copy = (struct gnl_simfs_inode *)malloc(sizeof(struct gnl_simfs_inode));
    GNL_NULL_CHECK(inode_copy, ENOMEM, NULL)

    // create a deep copy of the given inode
    inode_copy->btime = inode->btime;
    inode_copy->mtime = inode->mtime;
    inode_copy->atime = inode->atime;
    inode_copy->size = inode->size;

    GNL_CALLOC(inode_copy->name, strlen(inode->name) + 1, NULL)
    strcpy(inode_copy->name, inode->name);

    inode_copy->direct_ptr = inode->direct_ptr;
    inode_copy->locked = inode->locked;
    inode_copy->reference_count = inode->reference_count;
    inode_copy->active_hippie_pid = inode->active_hippie_pid;
    inode_copy->waiting_locker_pid = inode->waiting_locker_pid;

    // initialize condition variables
    int res = pthread_cond_init(&(inode_copy->file_access_available), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // set the last status change timestamp of the inode
    inode_copy->ctime = time(NULL);

    return inode_copy;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_inode_update(struct gnl_simfs_inode *inode, const struct gnl_simfs_inode *with) {
    GNL_NULL_CHECK(inode, EINVAL, -1)
    GNL_NULL_CHECK(with, EINVAL, -1)

    // update the size of the file within the inode
    inode->size = with->size;

    // update the pointer of the file within the inode
    inode->direct_ptr = with->direct_ptr;

    // set the last status change timestamp of the inode
    inode->ctime = time(NULL);

    return 0;
}

#include <gnl_macro_end.h>
