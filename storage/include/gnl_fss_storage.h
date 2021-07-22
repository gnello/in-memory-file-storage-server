#ifndef GNL_FSS_STORAGE_H
#define GNL_FSS_STORAGE_H

const int REPOL_FIFO = 0;
const int REPOL_LRU = 1;
const int REPOL_LFU = 2;

typedef struct gnl_fss_storage gnl_fss_storage;

gnl_fss_storage *gnl_fss_storage_init(int capacity, int limit, int replacement_policy);

void gnl_fss_storage_destroy(gnl_fss_storage *storage);

#endif //GNL_FSS_STORAGE_H