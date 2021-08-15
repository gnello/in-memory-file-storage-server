#include <stdlib.h>
#include <errno.h>
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

    // initialize lock
    int res = pthread_mutex_init(&(inode->mtx), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // initialize the others attributes
    inode->last_modify_time = 0;
    inode->size = 0;
    inode->mtx_owner = 0;
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

    // destroy the lock, on failure proceed anyway
    pthread_mutex_destroy(&(inode->mtx));

    // destroy the inode
    free(inode);
}

#include <gnl_macro_end.h>
