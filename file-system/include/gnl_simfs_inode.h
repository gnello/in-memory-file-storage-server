#ifndef GNL_SIMFS_INODE_H
#define GNL_SIMFS_INODE_H

#include "./gnl_simfs_inode_struct.h"

/**
 * Create a new gnl_simfs_inode.
 *
 * @param name  The name of the inode. This is necessary in order to
 *              find the inode into the file table or into other
 *              data structures.
 *
 * @return      Returns the new gnl_simfs_inode created on success,
 *              NULL otherwise.
 */
extern struct gnl_simfs_inode *gnl_simfs_inode_init(const char *name);

/**
 * Destroy the given gnl_simfs_inode. Attention! This invocation
 * will delete also the file pointed by the given inode. If you want
 * to delete an inode-copy, use gnl_simfs_inode_copy_destroy instead.
 *
 * @param inode The inode instance to destroy.
 */
extern void gnl_simfs_inode_destroy(struct gnl_simfs_inode *inode);

/**
 * Destroy the given gnl_simfs_inode but preserve the pointed file.
 *
 * @param inode The inode instance to destroy.
 */
extern void gnl_simfs_inode_copy_destroy(struct gnl_simfs_inode *inode);

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
 * Wait for a file to became available using condition variable.
 * Attention! This method will suspend the process execution
 * until the target file will become available.
 *
 * @param inode The inode instance containing the file where
 *              to wait the unlock.
 * @param mtx   The lock of the invoking process to release and
 *              subsequently re-acquire on the file unlock.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_wait_file_lockability(struct gnl_simfs_inode *inode, pthread_mutex_t *mtx);

/**
 * Increase the reference count of the given inode.
 *
 * @param inode The inode instance where to increase the reference count.
 * @param pid   The id of the process who wants to increase the refs.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_increase_refs(struct gnl_simfs_inode *inode, unsigned int pid);

/**
 * Decrease the reference count of the given inode.
 *
 * @param inode The inode instance where to decrease the reference count.
 * @param pid   The id of the process who wants to decrease the refs.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_decrease_refs(struct gnl_simfs_inode *inode, unsigned int pid);

/**
 * Check whether the inode has references to it.
 *
 * @param inode The inode instance to check.
 *
 * @return      Returns 1 if the inode has one or more references
 *              to it, 0 otherwise, if an error occurred, returns -1.
 */
extern int gnl_simfs_inode_has_refs(struct gnl_simfs_inode *inode);

/**
 * Check if the inode has any other pid references besides the given pid.
 *
 * @param inode The inode instance to check.
 * @param pid   The id of the process to use for the checking.
 *
 * @return      Returns 1 if the inode has one or more pid references
 *              in addition to the given pid, 0 otherwise, if an error
 *              occurred, returns -1.
 */
extern int gnl_simfs_inode_has_other_pid_refs(struct gnl_simfs_inode *inode, unsigned int pid);

/**
 * Lock the file pointed by the given inode. The file must be
 * in the unlocked state.
 *
 * @param inode The inode instance containing the file to lock.
 * @param pid   The id of the process who wants to lock the file.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_file_lock(struct gnl_simfs_inode *inode, unsigned int pid);

/**
 * Unlock the file pointed by the given inode. The file must be
 * previously locked by the given pid.
 *
 * @param inode The inode instance containing the file to unlock.
 * @param pid   The id of the process who wants to unlock the file.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_file_unlock(struct gnl_simfs_inode *inode, unsigned int pid);

/**
 * Increase the pending locks count of the given inode.
 *
 * @param inode The inode instance where to increase the
 *              pending locks count.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_increase_pending_locks(struct gnl_simfs_inode *inode);

/**
 * Decrease the pending locks count of the given inode.
 *
 * @param inode The inode instance where to decrease the
 *              pending locks count.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_decrease_pending_locks(struct gnl_simfs_inode *inode);

/**
 * Check whether the inode has pending_locks.
 *
 * @param inode The inode instance to check.
 *
 * @return      Returns 1 if the inode has one or more pending locks,
 *              0 otherwise, if an error occurred, returns -1.
 */
extern int gnl_simfs_inode_has_pending_locks(struct gnl_simfs_inode *inode);

/**
 * Write up to count bytes from the buffer starting at buf to the
 * file within the given inode. This method updates the given inode
 * size and mtime attributes.
 *
 * @param inode The inode instance where to write to the file.
 * @param buf   The buffer pointer containing the data to write.
 * @param count The count of bytes to write.
 *
 * @return      Returns the number of bytes wrote into the file on success,
 *              -1 otherwise.
 */
extern int gnl_simfs_inode_write(struct gnl_simfs_inode *inode, const void *buf, size_t count);

/**
 * Read the whole file within the given inode into the given buffer, and
 * write the number of bytes read into the given count. This method updates
 * the given inode atime attribute.
 *
 * @param inode The inode instance where to write to the file.
 * @param buf   The buffer pointer where to write the read data.
 * @param count The count of bytes read.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_read(struct gnl_simfs_inode *inode, void **buf, size_t *count);

/**
 * Create and return a copy of the given inode. The copy does not preserve
 * the original inode buffer and the original reference list.
 *
 * @param inode The inode instance to copy.
 *
 * @return      Returns a copy of the given inode on success, -1 otherwise.
 */
extern struct gnl_simfs_inode *gnl_simfs_inode_copy(const struct gnl_simfs_inode *inode);

/**
 * Flush the buffer of the given inode into his direct pointer. This method
 * will reset the buffer and will update the mtime, ctime, size and direct_ptr
 * attributes of the given inode.
 *
 * @param inode The inode to be flushed.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_inode_fflush(struct gnl_simfs_inode *inode);

#endif //GNL_SIMFS_INODE_H