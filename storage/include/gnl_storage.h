#ifndef GNL_FSS_STORAGE_H
#define GNL_FSS_STORAGE_H

const int REPOL_FIFO = 0;
const int REPOL_LRU = 1;
const int REPOL_LFU = 2;

struct gnl_storage;

extern struct gnl_storage *gnl_storage_init(int capacity, int limit, int replacement_policy);

extern void gnl_storage_destroy(struct gnl_storage *storage);

#endif //GNL_FSS_STORAGE_H