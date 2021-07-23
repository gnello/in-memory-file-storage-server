#ifndef GNL_FSS_STORAGE_H
#define GNL_FSS_STORAGE_H

const int REPOL_FIFO = 0;
const int REPOL_LRU = 1;
const int REPOL_LFU = 2;

struct gnl_storage;

extern struct gnl_storage *gnl_storage_init(int capacity, int limit, int replacement_policy);

extern void gnl_storage_destroy(struct gnl_storage *storage);

extern int gnl_storage_open(struct gnl_storage *storage, char *filename, int flags);

extern int gnl_storage_read(struct gnl_storage *storage, char *filename);

extern int gnl_storage_write(struct gnl_storage *storage, char *filename, char *data);

extern int gnl_storage_close(struct gnl_storage *storage, char *filename);

extern int gnl_storage_remove(struct gnl_storage *storage, char *filename);

#endif //GNL_FSS_STORAGE_H