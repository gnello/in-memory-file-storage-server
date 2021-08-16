#ifndef GNL_SIMFS_INODE_H
#define GNL_SIMFS_INODE_H

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

    // the direct pointer to the file
    char *direct_ptr;

    // the owner id of the lock, it should be a number > 0:
    // if 0 then the inode is unlocked, if > 0 the inode is locked;
    // we do not use native lock implementation here because
    // the lock status must persist between different methods invocations
    unsigned int locked;

    // the condition variable to track if a file is unlocked.
    pthread_cond_t file_unlocked;

    // the reference counter of the inode
    unsigned int reference_count;
};

/**
 * Create a new gnl_simfs_inode.
 *
 * @return  Returns the new gnl_simfs_inode created on success,
 *          NULL otherwise.
 */
extern struct gnl_simfs_inode *gnl_simfs_inode_init();

/**
 * Destroy the given gnl_simfs_inode.
 *
 * @param inode The inode instance to destroy.
 */
extern void gnl_simfs_inode_destroy(struct gnl_simfs_inode *inode);

/**
 * Check whether the file of the given inode is locked or not.
 *
 * @param inode The inode instance to check.
 *
 * @return      Returns the owner id of the lock if the file of the given
 *              inode is locked, 0 if it is unlocked, -1 on errors.
 */
extern int gnl_simfs_inode_is_file_locked(struct gnl_simfs_inode *inode);

/**
 * Wait for a file unlock using condition variable. Attention! This
 * method it will suspend the process execution until the target file
 * will be unlocked.
 *
 * @param inode The inode instance containing the file where
 *              to wait the unlock.
 * @param mtx   The lock of the invoking process to release and
 *              subsequently re-acquire on the file unlock.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_wait_unlock(struct gnl_simfs_inode *inode, pthread_mutex_t *mtx);

/**
 * Increase the reference count of the given inode.
 *
 * @param inode The inode instance where to increase the
 *              reference count.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_increase_refs(struct gnl_simfs_inode *inode);

/**
 * Decrease the reference count of the given inode.
 *
 * @param inode The inode instance where to decrease the
 *              reference count.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_decrease_refs(struct gnl_simfs_inode *inode);

#endif //GNL_SIMFS_INODE_H