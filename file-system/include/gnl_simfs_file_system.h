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
 * @return
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
 *
 * @param file_system
 * @param filename
 * @param flags
 *
 * @return
 */
extern int gnl_simfs_file_system_open(struct gnl_simfs_file_system *file_system, char *filename, int flags);

#endif //GNL_SIMFS_FILE_SYSTEM_H