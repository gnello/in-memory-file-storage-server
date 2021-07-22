#include "../include/gnl_fss_storage_service.h"
#include <gnl_macro_beg.h>

int gnl_fss_storage_service_open(struct gnl_storage *storage, char *filename, int flags) {
    return 0;
}

int gnl_fss_storage_service_read_n(struct gnl_storage *storage, int n) {
    return 0;
}

int gnl_fss_storage_service_read(struct gnl_storage *storage, char *filename) {
    return 0;
}

int gnl_fss_storage_service_write(struct gnl_storage *storage, char *filename, char *data) {
    return 0;
}

int gnl_fss_storage_service_append(struct gnl_storage *storage, char *filename, char *data) {
    return 0;
}

int gnl_fss_storage_service_lock(struct gnl_storage *storage, char *filename) {
    return 0;
}

int gnl_fss_storage_service_unlock(struct gnl_storage *storage, char *filename) {
    return 0;
}

int gnl_fss_storage_service_close(struct gnl_storage *storage, char *filename) {
    return 0;
}

int gnl_fss_storage_service_remove(struct gnl_storage *storage, char *filename) {
    return 0;
}

#include <gnl_macro_end.h>