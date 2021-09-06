#ifndef GNL_SIMFS_FILE_TABLE_H
#define GNL_SIMFS_FILE_TABLE_H

#include <gnl_list_t.h>

/**
 * The file table data structure.
 */
struct gnl_simfs_file_table;

/**
 * Create a new file table instance.
 *
 * @return      Returns the new gnl_simfs_file_table created on success,
 *              NULL otherwise.
 */
struct gnl_simfs_file_table *gnl_simfs_file_table_init();

/**
 * Destroy the given file table.
 *
 * @param table The file table instance to destroy.
 */
void gnl_simfs_file_table_destroy(struct gnl_simfs_file_table *table);

/**
 * Get the size in bytes of the given file table.
 *
 * @param file_table    The file table instance from where to get the size.
 *
 * @return              Returns the size in bytes of the given file table on
 *                      success, -1 otherwise.
 */
static unsigned long gnl_simfs_file_table_size(struct gnl_simfs_file_table *file_table);

/**
 * Get the file count of the given file table.
 *
 * @param file_table    The file table instance from where to get the file count.
 *
 * @return              Returns the file count of the given file table on success,
 *                      -1 otherwise.
 */
static int gnl_simfs_file_table_count(struct gnl_simfs_file_table *file_table);

/**
 * Get a copy of the file list of the given file table.
 *
 * @param file_table    The file table instance from where to get the file list.
 *
 * @return              Returns a copy of the file list of the given file table on success,
 *                      NULL otherwise.
 */
static struct gnl_list_t *gnl_simfs_file_table_list(struct gnl_simfs_file_table *file_table);

/**
 * Get the inode of the given filename.
 *
 * @param file_table    The file table instance from where to get the inode.
 * @param filename      The filename of the file pointed by the inode to get.
 *
 * @return              Returns the inode of the given filename on success,
 *                      NULL otherwise.
 */
static struct gnl_simfs_inode *gnl_simfs_file_table_get(struct gnl_simfs_file_table *file_table, const char *filename);

/**
 * Create a new file and put it into the given file table.
 *
 * @param file_table    The file table instance where to put the created file.
 * @param filename      The filename of the file to create.
 *
 * @return              Returns the inode of the created file on success,
 *                      NULL otherwise.
 */
static struct gnl_simfs_inode *gnl_simfs_file_table_create(struct gnl_simfs_file_table *file_table, const char *filename);

/**
 * Flush the given inode into the given file table.
 *
 * @param file_table    The file table instance where to flush the inode.
 * @param inode         The inode to use to flush. It must be a copy of an original
 *                      inode got from the file table.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int gnl_simfs_file_table_fflush(struct gnl_simfs_file_table *file_table, struct gnl_simfs_inode *new_inode);

/**
 * Remove an existing file table entry.
 *
 * @param file_table    The file table instance where to remove the entry.
 * @param key           The key of the entry to be removed.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int gnl_simfs_file_table_remove(struct gnl_simfs_file_table *file_table, const char *key);

#endif //GNL_SIMFS_FILE_TABLE_H