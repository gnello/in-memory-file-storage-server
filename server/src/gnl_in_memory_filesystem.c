#include <errno.h>
#include <gnl_list_t.h>
#include <gnl_queue_t.h>
#include "../include/gnl_in_memory_filesystem.h"

#define GNL_NULL_CHECK(ptr, error_code, return_value) { \
    if (ptr == NULL) {                                  \
        errno = error_code;                             \
                                                        \
        return return_value;                            \
    }                                                   \
}

struct storage_t {
    int capacity;
    int limit;
    int replacement_policy;
    gnl_list_t *storage;
    gnl_list_t *inode;
};

struct storage_t gnl_in_memory_filesystem_init(int capacity, int limit, int replacement_policy) {
    struct storage_t *filesystem = (struct storage_t *)malloc(sizeof(struct storage_t));
    GNL_NULL_CHECK(filesystem, ENOMEM, NULL);

    filesystem->capacity = capacity;
    filesystem->limit = limit;

    if (replacement_policy != REPOL_FIFO && replacement_policy != REPOL_LRU && replacement_policy != REPOL_LFU) {
        errno = EINVAL;

        return -1;
    }

    filesystem->replacement_policy = replacement_policy;

    return filesystem;
}