#include <errno.h>
#include <string.h>
#include "../include/gnl_simfs_file_table.h"
#include "./gnl_simfs_inode.c"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
struct gnl_simfs_file_table {

    // the file table data structure, contains all the
    // inodes of the files present into it
    struct gnl_ternary_search_tree_t *table;

    // the memory in bytes allocated by the file table
    unsigned long size;

    // the counter of the files present in the file table
    int count;
};

/**
 * Destroy an inode. It should be passed to the gnl_ternary_search_tree_destroy
 * method of the gnl_ternary_search_tree data structure.
 *
 * @param ptr   The void pointer value of the gnl_ternary_search_tree data structure.
 */
static void destroy_file_table_inode(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    // implicitly cast the value
    struct gnl_simfs_inode *inode = ptr;

    // destroy the obtained inode
    gnl_simfs_inode_destroy(inode);
}

/**
 * {@inheritDoc}
 */
struct gnl_simfs_file_table *gnl_simfs_file_table_init() {
    struct gnl_simfs_file_table *t = (struct gnl_simfs_file_table *)malloc(sizeof(struct gnl_simfs_file_table));
    GNL_NULL_CHECK(t, ENOMEM, NULL)

    // instantiate the table to NULL, the table
    // space will be allocated on demand
    t->table = NULL;

    // initialize the size
    t->size = 0;

    // initialize the count
    t->count = 0;

    return t;
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_file_table_destroy(struct gnl_simfs_file_table *table) {
    if (table == NULL) {
        return;
    }

    // destroy the ternary search tree
    gnl_ternary_search_tree_destroy(&(table->table), destroy_file_table_inode);

    // destroy the table
    free(table);
}

/**
 * {@inheritDoc}
 */
static unsigned long gnl_simfs_file_table_size(struct gnl_simfs_file_table *file_table) {
    // validate the parameters
    GNL_NULL_CHECK(file_table, EINVAL, -1)

    return file_table->size;
}

/**
 * {@inheritDoc}
 */
static int gnl_simfs_file_table_count(struct gnl_simfs_file_table *file_table) {
    // validate the parameters
    GNL_NULL_CHECK(file_table, EINVAL, -1)

    return file_table->count;
}

/**
 * {@inheritDoc}
 */
static struct gnl_simfs_inode *gnl_simfs_file_table_get(struct gnl_simfs_file_table *file_table, const char *filename) {
    // validate the parameters
    GNL_NULL_CHECK(file_table, EINVAL, NULL)
    GNL_NULL_CHECK(filename, EINVAL, NULL)

    struct gnl_simfs_inode *inode;

    // search the key in the file table
    void *raw_inode = gnl_ternary_search_tree_get(file_table->table, filename);

    // if the key is not present return an error
    GNL_NULL_CHECK(raw_inode, ENOENT, NULL)

    // else cast the raw_inode
    inode = (struct gnl_simfs_inode *)raw_inode;

    return inode;
}

/**
 * {@inheritDoc}
 */
static struct gnl_simfs_inode *gnl_simfs_file_table_create(struct gnl_simfs_file_table *file_table, const char *filename) {
    // validate the parameters
    GNL_NULL_CHECK(file_table, EINVAL, NULL)
    GNL_NULL_CHECK(filename, EINVAL, NULL)

    // search the filename key in the file table
    struct gnl_simfs_inode *tmp = gnl_simfs_file_table_get(file_table, filename);

    // if the key is present return an error
    GNL_MINUS1_CHECK(-1 * (tmp != NULL), EEXIST, NULL)

    // create a new inode
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init(filename);
    GNL_NULL_CHECK(inode, errno, NULL)

    // put the inode into the file table
    int res = gnl_ternary_search_tree_put(&(file_table->table), filename, inode);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // increment the files counter
    file_table->count++;

    return inode;
}

/**
 * {@inheritDoc}
 */
static int gnl_simfs_file_table_fflush(struct gnl_simfs_file_table *file_table,
        const struct gnl_simfs_inode *buffer_entry, size_t count) {
    // validate the parameters
    GNL_NULL_CHECK(file_table, EINVAL, -1)
    GNL_NULL_CHECK(buffer_entry, EINVAL, -1)

    // search the key in the file table
    struct gnl_simfs_inode *inode = gnl_simfs_file_table_get(file_table, buffer_entry->name);

    // if the key is not present return an error
    GNL_NULL_CHECK(inode, errno, -1)

    // check if the buffer_entry is not the original inode
    if (inode == buffer_entry) {
        errno = EINVAL;

        return -1;
    }

    // update the inode with the new entry
    int res = gnl_simfs_inode_update(inode, buffer_entry, count);
    GNL_MINUS1_CHECK(res, errno, -1)

    // update the file table size
    file_table->size += count;

    return 0;
}

/**
 * {@inheritDoc}
 */
static int gnl_simfs_file_table_remove(struct gnl_simfs_file_table *file_table, const char *key) {
    // validate the parameters
    GNL_NULL_CHECK(file_table, EINVAL, -1)
    GNL_NULL_CHECK(key, EINVAL, -1)

    // search the key in the file table
    struct gnl_simfs_inode *inode = gnl_simfs_file_table_get(file_table, key);

    // if the key is not present return an error
    GNL_NULL_CHECK(inode, errno, -1)

    // get the size of the inode
    int count = inode->size;

    // remove the file
    int res = gnl_ternary_search_tree_remove(file_table->table, key, destroy_file_table_inode);
    GNL_MINUS1_CHECK(res, errno, -1)

    // update the file table size
    file_table->size -= count;

    // update the file table count
    file_table->count--;

    return 0;
}

#include <gnl_macro_end.h>
