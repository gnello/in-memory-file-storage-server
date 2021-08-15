#ifndef GNL_SIMFS_INODE_H
#define GNL_SIMFS_INODE_H

#include <time.h>
#include <pthread.h>

/**
 * File's inode for the Simplified In Memory File System (SIMFS).
 */
struct gnl_simfs_inode {

    // the creation time of the inode
    time_t creation_time;

    // the last modify time of the inode
    time_t last_modify_time;

    // the size of the pointed file
    unsigned int size;
    gnl_simfs_file_system.h
    // the direct pointer to the file
    char *direct_ptr;

    // the lock of the inode
    pthread_mutex_t mtx;

    // the owner id of the lock, it should be a number > 0
    int mtx_owner;
};

/**
 * Create a new gnl_simfs_inode.
 *
 * @return  Returns the new gnl_simfs_inode created on success,
 *          NULL otherwise.
 */
extern struct gnl_simfs_inode gnl_simfs_inode_init();

/**
 * Destroy the given gnl_simfs_inode.
 *
 * @param inode The inode instance to destroy.
 */
extern void gnl_simfs_inode_destroy(struct gnl_simfs_inode *inode);

#endif //GNL_SIMFS_INODE_H