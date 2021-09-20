#ifndef GNL_SIMFS_FILE_SYSTEM_H
#define GNL_SIMFS_FILE_SYSTEM_H

#include "./gnl_simfs_inode_struct.h"
#include <gnl_list_t.h>

/**
 * Passed to the gnl_simfs_file_system_open method
 * in order to create a file.
 */
const int GNL_SIMFS_O_CREATE = 1;

/**
 * Passed to the gnl_simfs_file_system_open method
 * to atomically open a file with a lock.
 */
const int GNL_SIMFS_O_LOCK = 2;

/**
 * The possible type of replacement policy to adopt
 * in case the file system reaches the max heap or
 * the max number of files. In this case one or more
 * file/s will be evicted in accordance with the
 * policy chosen.
 */
enum gnl_simfs_replacement_policy {

    // no file will be evicted
    GNL_SIMFS_RP_NONE,

    // evict the least recently created file
    GNL_SIMFS_RP_FIFO,

    // evict the most recently created file
    GNL_SIMFS_RP_LIFO,

    // evict the least recently used file
    GNL_SIMFS_RP_LRU,

    // evict the most recently used file
    GNL_SIMFS_RP_MRU,

    // evict the least often used file
    GNL_SIMFS_RP_LFU,
};

/**
 * The file system structure.
 */
struct gnl_simfs_file_system;

/**
 * The evicted file structure. This structure is
 * returned as an element of the evicted list set during
 * an open or write operations.
 */
struct gnl_simfs_evicted_file {

    // the buffer of bytes evicted
    void *bytes;

    // the number of bytes evicted
    size_t count;
};

/**
 * Create a new simple in-memory file system instance.
 *
 * @param memory_limit          The maximum memory allocable in megabyte by the file system.
 *                              If the underlying volume reaches this limit, it will be considered full.
 *                              If 0, the file system will not be limited.
 * @param files_limit           The maximum number of files that can be handled by the file system.
 *                              If 0, the file system can handle virtually an infinite number of files.
 * @param log_path              The log path where to write the log. If NULL is passed, no logging
 *                              will be performed.
 * @param log_level             The wanted log level. Accepted values: NULL, trace, debug, info, warn, error.
 * @param replacement_policy    The replacement policy to adopt in case the file system reaches the given
 *                              memory_limit or the given file_limit. In this case one or more file/s will be
 *                              evicted in accordance with the policy chosen.
 *
 * @return                      Returns the new gnl_simfs_file_system created on success,
 *                              NULL otherwise.
 */
extern struct gnl_simfs_file_system *gnl_simfs_file_system_init(unsigned int memory_limit, unsigned int file_limit,
        const char *log_path, const char *log_level, enum gnl_simfs_replacement_policy replacement_policy);

/**
 * Destroy the given file system. Every file into it will be lost, all the allocated memory
 * will be freed.
 *
 * @param file_system   The file system instance to destroy.
 */
extern void gnl_simfs_file_system_destroy(struct gnl_simfs_file_system *file_system);

/**
 * Open the file pointed by the given filename and return a file descriptor referring
 * to it. Multiple invocations on this method from the same process will obtain
 * different file descriptors.
 *
 * @param file_system   The file system instance from where to get the file.
 * @param filename      The filename of the file to open.
 * @param flags         If GNL_SIMFS_O_CREATE is given, the file will be created,
 *                      if GNL_SIMFS_O_LOCK is given, the file will be opened in
 *                      locked mode.
 * @param pid           The id of the process who invoked this method.
 * @param evicted_list  The pointer to the list where to put the eventual evicted
 *                      files in accordance with the replacement policy given at
 *                      the moment of the file system initialization.
 *
 * @return              Returns a file descriptor referring to the opened file
 *                      on success, -1 otherwise.
 */
extern int gnl_simfs_file_system_open(struct gnl_simfs_file_system *file_system, const char *filename, int flags,
        unsigned int pid, struct gnl_list_t **evicted_list);

/**
 * Write up to count bytes from the buffer starting at buf to the file referred to by
 * the file descriptor fd.
 *
 * @param file_system   The file system instance where to write the file.
 * @param fd            The file descriptor referring the file where to write.
 * @param buf           The buffer pointer containing the data to write.
 * @param count         The count of bytes to write.
 * @param pid           The id of the process who invoked this method.
 * @param evicted_list  The pointer to the list where to put the eventual evicted
 *                      files in accordance with the replacement policy given at
 *                      the moment of the file system initialization.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
extern int gnl_simfs_file_system_write(struct gnl_simfs_file_system *file_system, int fd, const void *buf, size_t count,
        unsigned int pid, struct gnl_list_t **evicted_list);

/**
 * Read the whole file pointed by the given file descriptor fd into buf, and
 * write the number of bytes read into count.
 *
 * @param file_system   The file system instance from where to read the file.
 * @param fd            The file descriptor referring the file to read.
 * @param buf           The buffer pointer where to write the read data.
 * @param count         The count of bytes read.
 * @param pid           The id of the process who invoked this method.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
extern int gnl_simfs_file_system_read(struct gnl_simfs_file_system *file_system, int fd, void **buf, size_t *count,
        unsigned int pid);

/**
 * Close the given file descriptor. After this invocation the given file descriptor will no
 * longer be valid.
 *
 * @param file_system   The file system instance where to close the given file descriptor.
 * @param fd            The file descriptor to close.
 * @param pid           The id of the process who invoked this method.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
extern int gnl_simfs_file_system_close(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid);

/**
 * Remove the file pointed by the given filename. After this invocation the file removed
 * can not be accessed anymore as it will completely destroyed.
 * To call this method, you must have called gnl_simfs_file_system_lock first.
 *
 * @param file_system   The file system instance where to remove the given file name.
 * @param filename      The filename of the file to remove.
 * @param pid           The id of the process who invoked this method.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
extern int gnl_simfs_file_system_remove(struct gnl_simfs_file_system *file_system, const char *filename, unsigned int pid);

/**
 * Lock the file pointed by the given filename. After this invocation the file locked
 * can not be accessed by other pid.
 *
 * @param file_system   The file system instance where to lock the given file name.
 * @param fd            The file descriptor to lock.
 * @param pid           The id of the process who invoked this method.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
extern int gnl_simfs_file_system_lock(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid);

/**
 * Unlock the file pointed by the given filename. After this invocation the file unlocked
 * can be accessed by any other pid.
 *
 * @param file_system   The file system instance where to unlock the given file name.
 * @param fd            The file descriptor to unlock.
 * @param pid           The id of the process who invoked this method.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
extern int gnl_simfs_file_system_unlock(struct gnl_simfs_file_system *file_system, int fd, unsigned int pid);

/**
 * Get the list of all files present into the file system.
 *
 * @param file_system   The file system instance where to get a list of files.
 * @param pid           The id of the process who invoked this method.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
extern struct gnl_list_t *gnl_simfs_file_system_ls(struct gnl_simfs_file_system *file_system, unsigned int pid);

/**
 * Get a copy of the inode of the given filename. The inode returned is put
 * into the given buf.
 *
 * @param file_system   The file system instance where to get the inode.
 * @param filename      The filename of the file to get the information.
 * @param buf           The pointer to use to get the inode.
 * @param pid           The id of the process who invoked this method.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
extern int gnl_simfs_file_system_stat(struct gnl_simfs_file_system *file_system, const char *filename,
        struct gnl_simfs_inode *buf, unsigned int pid);

/**
 * Get a copy of the inode of the given fd. The inode returned is put
 * into the given buf.
 *
 * @param file_system   The file system instance where to get the inode.
 * @param fd            The file descriptor of which get the inode.
 * @param buf           The pointer to use to get the inode.
 * @param pid           The id of the process who invoked this method.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
extern int gnl_simfs_file_system_fstat(struct gnl_simfs_file_system *file_system, int fd, struct gnl_simfs_inode *buf,
        unsigned int pid);

#endif //GNL_SIMFS_FILE_SYSTEM_H