#ifndef GNL_SIMFS_FILE_DESCRIPTOR_TABLE_H
#define GNL_SIMFS_FILE_DESCRIPTOR_TABLE_H

#include "./gnl_simfs_inode.h"

/**
 * The file descriptor table structure.
 */
struct gnl_simfs_file_descriptor_table;

/**
 * Create a new file descriptor table instance.
 *
 * @param limit The maximum number of files that the file descriptor table
 *              can handle simultaneously.
 *
 * @return      Returns the new gnl_simfs_file_descriptor_table created on success,
 *              NULL otherwise.
 */
extern struct gnl_simfs_file_descriptor_table *gnl_simfs_file_descriptor_table_init(unsigned int limit);

/**
 * Destroy the given file descriptor table.
 *
 * @param file_system   The file descriptor table instance to destroy.
 */
extern void gnl_simfs_file_descriptor_table_destroy(struct gnl_simfs_file_descriptor_table *table);

/**
 * Insert an inode into the given file descriptor table.
 *
 * @param table The file descriptor table instance where to put the inode.
 * @param inode The inode to insert into the given file descriptor table.
 *
 * @return      On success, returns the file descriptor of the file within
 *              the inserted inode, on failure returns -1.
 */
extern int gnl_simfs_file_descriptor_table_put(struct gnl_simfs_file_descriptor_table *table, const struct gnl_simfs_inode *inode);

/**
 * Remove a file descriptor from the given file descriptor table. It will not delete
 * the associated inode.
 *
 * @param table The file descriptor table instance from where delete the file descriptor.
 * @param fd    The file descriptor to remove from the given file descriptor table.
 *
 * @return      Return 0 on success, -1 otherwise.
 */
extern int gnl_simfs_file_descriptor_table_remove(struct gnl_simfs_file_descriptor_table *table, unsigned int fd);

/**
 * Get the current size of the given file descriptor table.
 *
 * @param table The file descriptor table instance.
 *
 * @return  Returns the size of the given file descriptor table
 *          on success, -1 on failure.
 */
extern int gnl_simfs_file_descriptor_table_size(struct gnl_simfs_file_descriptor_table *table);

#endif //GNL_SIMFS_FILE_DESCRIPTOR_TABLE_H