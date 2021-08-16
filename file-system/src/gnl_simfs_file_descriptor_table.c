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

    // the greater file descriptor currently
    // present into the table
    int max_fd;

    // the map of the free file descriptor table index available,
    // it becomes useful to implement the "first fit" policy in
    // case an inode was removed from the table and a new one
    // must be inserted
    struct gnl_min_heap_t *free_index_map;

    // the inodes array, it will grow dynamically
    // until the given limit is reached; a removed
    // element does not cause a table shrink, but it
    // creates a "hole" that may will be filled on the
    // next put thankful to the "first fit" policy
    struct gnl_simfs_inode **table;
};

/**
 * {@inheritDoc}
 */
struct gnl_simfs_file_descriptor_table *gnl_simfs_file_descriptor_table_init(int limit) {
    struct gnl_simfs_file_descriptor_table *t = (struct gnl_simfs_file_descriptor_table *)malloc(sizeof(struct gnl_simfs_file_descriptor_table));
    GNL_NULL_CHECK(t, ENOMEM, NULL)

    // assign the limit
    t->limit = limit;

    // initialize the size
    t->size = 0;

    // initialize the max fd
    t->max_fd = 0;

    // initialize the free index map
    t->free_index_map = gnl_min_heap_init();
    GNL_NULL_CHECK(t->free_index_map, errno, NULL)

    // the table space will be allocated on demand
    t->table = NULL;

    return t;
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_file_descriptor_table_destroy(struct gnl_simfs_file_descriptor_table *table) {
    if (table == NULL) {
        return;
    }

    // if the table is not empty...
    if (table->size > 0) {

        // we do not want to free the "holes" left by a remove
        // (because they are already been removed), so create a bitmap
        // to map the elements to be freed: if 1 the element is still
        // present in the table, if 0 the element was present but not anymore
        int bitmap[table->max_fd];

        // initialize the bitmap
        for (size_t i=0; i<=table->max_fd; i++) {
            bitmap[i] = 1;
        }

        // fill the bitmap
        void *raw_fd;
        while ((raw_fd = gnl_min_heap_extract_min(table->free_index_map)) != NULL) {
            bitmap[*(int *)raw_fd] = 0;

            // free the deep copy of the fd
            free(raw_fd);
        }

        // destroy the table elements
        for (size_t i=0; i<=table->max_fd; i++) {
            if (bitmap[i] > 0) {
                free(table->table[i]);
            }
        }
    }

    // destroy the table
    free(table->table);

    // destroy the free index map, the free callback is
    // necessary for the cases where the table has size 0,
    // but its free index map contains some fds
    gnl_min_heap_destroy(table->free_index_map, free);

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

        // free the deep copy of the fd
        free(raw_fd);
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
int gnl_simfs_file_descriptor_table_put(struct gnl_simfs_file_descriptor_table *table, const struct gnl_simfs_inode *inode) {
    GNL_NULL_CHECK(table, EINVAL, -1)

    // check if we can insert another element
    if (table->size == table->limit) {
        errno = EMFILE;

        return -1;
    }

    // get the file descriptor
    int fd = get_file_descriptor(table);

    // reallocate space for the table if necessary, there are three cases:
    // - case 1 (fd == 0):              this is the first use of the table, so a
    //                                  memory allocation is mandatory
    // - case 2 (fd > table->max_fd):   the table needs extra space for the new fd
    //                                  because it has reached its border
    // - case 3 (none of the above):    there is a "hole" left by a previous remove,
    //                                  so the table already has the necessary space
    if (fd == 0 || fd > table->max_fd) {
        struct gnl_simfs_inode **temp = realloc(table->table, (fd + 1) * sizeof(struct gnl_simfs_inode *));
        GNL_NULL_CHECK(temp, errno, -1)

        table->table = temp;
    }

    // allocate space for the new inode
    *(table->table + fd) = (struct gnl_simfs_inode *)malloc(sizeof(struct gnl_simfs_inode));
    GNL_NULL_CHECK(*(table->table + fd), ENOMEM, -1)

    // insert a deep copy of the given inode
    **(table->table + fd) = *inode;

    // increase the table size
    table->size++;

    // update the max fd
    if (fd > table->max_fd) {
        table->max_fd = fd;
    }

    // return the file descriptor
    return fd;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_file_descriptor_table_remove(struct gnl_simfs_file_descriptor_table *table, int fd) {
    GNL_NULL_CHECK(table, EINVAL, -1)

    if (table->size == 0) {
        errno = EPERM;

        return -1;
    }

    // remove the file descriptor
    free(table->table[fd]);

    // if the fd is the maximum active file descriptor decrease it by one
    if (fd == table->max_fd) {
        table->max_fd--;
    }
    // else add the removed file descriptor into the free index map
    else {
        // make a deep copy of the fd
        int *fd_copy = malloc(sizeof(int));
        GNL_NULL_CHECK(fd_copy, ENOMEM, -1)

        *fd_copy = fd;

        // add the deep copy into the free index map
        int res = gnl_min_heap_insert(table->free_index_map, fd_copy, fd);
        GNL_MINUS1_CHECK(res, errno, -1)
    }

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
