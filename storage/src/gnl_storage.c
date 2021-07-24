#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <gnl_list_t.h>
#include "../include/gnl_storage.h"
#include <gnl_macro_beg.h>

/**
 * In memory storage implementation.
 */
struct gnl_storage {
    int capacity;
    int limit;
    int replacement_policy;
    gnl_list_t *storage;
    gnl_list_t *inode;
};

/**
 * Describes a file.
 */
struct gnl_fss_inode {
    int id;
    time_t timestamp;
    time_t last_access;
    time_t last_modify;
    int file_size;
    int file_ptr;
};
// https://en.wikipedia.org/wiki/File_descriptor
struct gnl_fss_file_descriptor { //punta alla tabella dei file
    int id;
    struct gnl_fss_inode *inode;
};

//TODO: hashtable di inode

struct gnl_storage *gnl_storage_init(int capacity, int limit, int replacement_policy) {
    struct gnl_storage *storage = (struct gnl_storage *)malloc(sizeof(struct gnl_storage));
    GNL_NULL_CHECK(storage, ENOMEM, NULL);

    storage->capacity = capacity;
    storage->limit = limit;

    switch (replacement_policy) {
        case 0: // FIFO
            // no break
        case 1: // LRU
            // no break
        case 2: // LFU
            break;

        default:
            errno = EINVAL;
            return NULL;
            /* NOT REACHED */
            break;
    }

    storage->replacement_policy = replacement_policy;

    return storage;
}

void gnl_storage_destroy(struct gnl_storage *storage) {
    if (storage != NULL) {
        free(storage);
    }
}

#include <gnl_macro_end.h>