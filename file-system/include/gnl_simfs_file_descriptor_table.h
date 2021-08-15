#ifndef GNL_SIMFS_FILE_DESCRIPTOR_TABLE_H
#define GNL_SIMFS_FILE_DESCRIPTOR_TABLE_H

#include "./gnl_simfs_inode.h"

/**
 * The file descriptor table structure.
 */
struct gnl_simfs_file_descriptor_table;


extern struct gnl_simfs_file_descriptor_table *gnl_simfs_file_descriptor_table_init(int limit);

extern void gnl_simfs_file_descriptor_table_destroy(struct gnl_simfs_file_descriptor_table *table);

extern int gnl_simfs_file_descriptor_table_put(struct gnl_simfs_file_descriptor_table *table, struct gnl_simfs_inode *inode);

extern int gnl_simfs_file_descriptor_table_remove(struct gnl_simfs_file_descriptor_table *table, struct gnl_simfs_inode *inode);

extern int gnl_simfs_file_descriptor_table_size(struct gnl_simfs_file_descriptor_table *table);

#endif //GNL_SIMFS_FILE_DESCRIPTOR_TABLE_H