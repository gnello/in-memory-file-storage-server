#ifndef GNL_IN_MEMORY_FILESYSTEM_H
#define GNL_IN_MEMORY_FILESYSTEM_H

const int REPOL_FIFO = 0;
const int REPOL_LRU = 1;
const int REPOL_LFU = 2;

struct gnl_storage_t *gnl_in_memory_filesystem_init(int capacity, int limit, int replacement_policy);

#endif //GNL_IN_MEMORY_FILESYSTEM_H