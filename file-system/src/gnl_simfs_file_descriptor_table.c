#include <stdlib.h>
#include <errno.h>
#include <gnl_min_heap_t.h>
#include "../include/gnl_simfs_file_descriptor_table.h"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
struct gnl_simfs_file_descriptor_table {

    // the number of files that the file descriptor table
    // can handle simultaneously
    int limit;

    // the current size of the file descriptor table
    int size;

    // the map of the free file descriptor table index available,
    // it becomes useful to implement the "first fit" policy in
    // case an inode was removed from the table and a new one
    // must be inserted
    struct gnl_min_heap_t *free_index_map;

    // the inodes array, it will grow dynamically
    // until the given limit is reached
    struct gnl_simfs_inode *table[];
};

/**
 * {@inheritDoc}
 */
struct gnl_simfs_file_descriptor_table *gnl_simfs_file_descriptor_table_init(int limit) {
    struct gnl_simfs_file_descriptor_table *t = (struct gnl_simfs_file_descriptor_table *)malloc(sizeof(struct gnl_simfs_file_descriptor_table));
    GNL_NULL_CHECK(t, ENOMEM, NULL)

    // assign the limit
    t->limit = limit;

    // initialize the free index map
    t->free_index_map = gnl_min_heap_init();
    GNL_NULL_CHECK(t->free_index_map, errno, NULL)

    return t;
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_file_descriptor_table_destroy(struct gnl_simfs_file_descriptor_table *table) {
    // destroy the free index map
    gnl_min_heap_destroy(table->free_index_map, NULL);

    free(table);
}

/**
 * Get the first file descriptor available in the given
 * file descriptor table, implementing the "first fit" policy.
 *
 * @param table The file descriptor table instance.
 *
 * @return      Returns a file descriptor.
 */
static int get_file_descriptor(struct gnl_simfs_file_descriptor_table *table) {
    int fd;

    // check if there is a free index into the free index map
    void *raw_fd = gnl_min_heap_extract_min(table->free_index_map);

    // if there is a free index...
    if (raw_fd != NULL) {
        // cast the file descriptor
        fd = *(int *)raw_fd;
    }
    // if there is not a free index...
    else {
        fd = table->size;
    }

    return fd;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_descriptor_table_put(struct gnl_simfs_file_descriptor_table *table, struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(table, EINVAL, -1)

    // check if we can insert another element
    if (table->size == table->limit) {
        errno = EMFILE;

        return -1;
    }

    // get the file descriptor
    int fd = get_file_descriptor(table);

    // allocate space for the new inode
    table->table[fd] = (struct gnl_simfs_inode *)malloc(sizeof(struct gnl_simfs_inode));
    GNL_NULL_CHECK(table->table[fd], ENOMEM, -1)

    // insert the given inode
    table->table[fd] = inode;

    // increase the table size
    table->size++;

    // return the file descriptor
    return fd;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_descriptor_table_remove(struct gnl_simfs_file_descriptor_table *table, int fd) {
    GNL_NULL_CHECK(table, EINVAL, -1)

    // remove the file descriptor
    free(table->table[fd]);

    // add the removed file descriptor into the free index map
    int res = gnl_min_heap_insert(table->free_index_map, (void *)&fd, fd);
    GNL_MINUS1_CHECK(res, errno, -1)

    // decrease the table size
    table->size--;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_descriptor_table_size(struct gnl_simfs_file_descriptor_table *table) {
    GNL_NULL_CHECK(table, EINVAL, -1)

    return table->size;
}

#include <gnl_macro_end.h>
