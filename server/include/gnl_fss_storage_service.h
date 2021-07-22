
#ifndef GNL_FSS_STORAGE_SERVICE_H
#define GNL_FSS_STORAGE_SERVICE_H

#include <gnl_storage.h>

extern int gnl_fss_storage_service_open(struct gnl_storage *storage, char *filename, int flags);

extern int gnl_fss_storage_service_read_n(struct gnl_storage *storage, int n);

extern int gnl_fss_storage_service_read(struct gnl_storage *storage, char *filename);

extern int gnl_fss_storage_service_write(struct gnl_storage *storage, char *filename, char *data);

extern int gnl_fss_storage_service_append(struct gnl_storage *storage, char *filename, char *data);

extern int gnl_fss_storage_service_lock(struct gnl_storage *storage, char *filename);

extern int gnl_fss_storage_service_unlock(struct gnl_storage *storage, char *filename);

extern int gnl_fss_storage_service_close(struct gnl_storage *storage, char *filename);

extern int gnl_fss_storage_service_remove(struct gnl_storage *storage, char *filename);

#endif //GNL_FSS_STORAGE_SERVICE_H