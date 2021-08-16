#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "../include/gnl_simfs_inode.h"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
struct gnl_simfs_inode *gnl_simfs_inode_init() {
    struct gnl_simfs_inode *inode = (struct gnl_simfs_inode *)malloc(sizeof(struct gnl_simfs_inode));
    GNL_NULL_CHECK(inode, ENOMEM, NULL)

    // set the creation time
    inode->creation_time = time(NULL);

    // initialize condition variables
    int res = pthread_cond_init(&(inode->file_unlocked), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // initialize others attributes
    inode->last_modify_time = 0;
    inode->size = 0;
    inode->locked = 0;
    inode->direct_ptr = NULL;

    return inode;
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_inode_destroy(struct gnl_simfs_inode *inode) {
    if (inode == NULL) {
        return;
    }

    // destroy the file pointer
    free(inode->direct_ptr);

    // destroy the condition variables
    pthread_cond_destroy(&(inode->file_unlocked));

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
int gnl_simfs_inode_wait_unlock(struct gnl_simfs_inode *inode, pthread_mutex_t *mtx) {
    return pthread_cond_wait(&(inode->file_unlocked), mtx);
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

#include <gnl_macro_end.h>
