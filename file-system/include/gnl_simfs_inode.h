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

    // the condition variable to track if a file can be locked or
    // read and written by a hippie pid.
    pthread_cond_t file_access_available;

    // the reference count of the inode
    unsigned int reference_count;

    // the count of pid that opened the pointed file
    // without any lock
    unsigned int active_hippie_pid;

    // the count of pid that want to lock the pointed file
    unsigned int waiting_locker_pid;
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
 *              inode is locked, 0 if it is unlocked, -1 on error.
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

/**
 * Lock the file pointed by the given inode. The file must be
 * in unlocked.
 *
 * @param inode The inode instance containing the file
 *              to lock.
 * @param pid   The id of the process who wants to lock
 *              the file.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_file_lock(struct gnl_simfs_inode *inode, unsigned int pid);

/**
 * Unlock the file pointed by the given inode. The file must be
 * previously locked by the given pid.
 *
 * @param inode The inode instance containing the file
 *              to unlock.
 * @param pid   The id of the process who wants to unlock
 *              the file.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_file_unlock(struct gnl_simfs_inode *inode, unsigned int pid);

/**
 * Increase the hippie pid count of the given inode.
 *
 * @param inode The inode instance where to increase the
 *              hippie pid count.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_increase_hippie_pid(struct gnl_simfs_inode *inode);

/**
 * Decrease the hippie pid count of the given inode.
 *
 * @param inode The inode instance where to decrease the
 *              hippie pid count.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_decrease_hippie_pid(struct gnl_simfs_inode *inode);

/**
 * Check whether the inode has active hippie pid.
 *
 * @param inode The inode instance to check.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_has_hippie_pid(struct gnl_simfs_inode *inode);

/**
 * Increase the locker pid count of the given inode.
 *
 * @param inode The inode instance where to increase the
 *              locker pid count.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_increase_locker_pid(struct gnl_simfs_inode *inode);

/**
 * Decrease the locker pid count of the given inode.
 *
 * @param inode The inode instance where to decrease the
 *              locker pid count.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_decrease_locker_pid(struct gnl_simfs_inode *inode);

/**
 * Check whether the inode has waiting locker pid.
 *
 * @param inode The inode instance to check.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_has_locker_pid(struct gnl_simfs_inode *inode);

#endif //GNL_SIMFS_INODE_H