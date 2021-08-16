#ifndef GNL_SIMFS_FILE_SYSTEM_H
#define GNL_SIMFS_FILE_SYSTEM_H

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
 * The file system structure.
 */
struct gnl_simfs_file_system;

/**
 * Create a new simple in-memory file system instance.
 *
 * @param memory_limit  The maximum memory allocable in megabyte by the file system.
 *                      If the underlying volume reaches this limit, it will be considered full.
 *                      If 0, the file system will not be limited.
 * @param files_limit   The maximum number of files that can be handled by the file system.
 *                      If 0, the file system can handle virtually an infinite number of files.
 *
 * @return              Returns the new gnl_simfs_file_system created on success,
 *                      NULL otherwise.
 */
extern struct gnl_simfs_file_system *gnl_simfs_file_system_init(unsigned int memory_limit, unsigned int file_limit);

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
 *
 * @return              Returns a file descriptor referring to the opened file
 *                      on success, -1 otherwise.
 */
extern int gnl_simfs_file_system_open(struct gnl_simfs_file_system *file_system, char *filename, int flags, unsigned int pid);

#endif //GNL_SIMFS_FILE_SYSTEM_H