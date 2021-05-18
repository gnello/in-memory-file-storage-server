#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <gnl_list_t.h>
#include "../include/gnl_fss_storage.h"

#define GNL_NULL_CHECK(ptr, error_code, return_value) { \
    if (ptr == NULL) {                                  \
        errno = error_code;                             \
                                                        \
        return return_value;                            \
    }                                                   \
}

struct gnl_fss_storage {
    int capacity;
    int limit;
    int replacement_policy;
    gnl_list_t *storage;
    gnl_list_t *inode;
};

struct gnl_fss_inode {
    int id;
    time_t timestamp;
    time_t last_access;
    time_t last_modify;
    int file_size;
    int file_ptr;
};

//TODO: hashtable di inode

gnl_fss_storage *gnl_fss_storage_init(int capacity, int limit, int replacement_policy) {
    gnl_fss_storage *storage = (struct gnl_fss_storage *)malloc(sizeof(struct gnl_fss_storage));
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

void gnl_fss_storage_destroy(gnl_fss_storage *storage) {
    if (storage != NULL) {
        free(storage);
    }
}

int gnl_fss_storage_open(const char *pathname) {
    return 0;
}

#undef GNL_NULL_CHECK