#define _POSIX_C_SOURCE 199309L

#include "../include/gnl_fss_api.h"
#include "./gnl_fss_socket_request.c" //TODO: far diventare libreria statica?
#include "./macro_beg.c"

int gnl_fss_api_open_connection(const char *sockname, int msec, const struct timespec abstime) {
    return 0;
}

int gnl_fss_api_close_connection(const char *sockname) {
    return 0;
}

int gnl_fss_api_open_file(const char *pathname, int flags) {
    //TODO: check params

    struct gnl_fss_socket_request *request = gnl_fss_socket_request_init(GNL_FSS_SOCKET_REQUEST_OPEN, 2, pathname, flags);
    GNL_NULL_CHECK(request, ENOMEM, -1)

    char *message = NULL;

    int res = gnl_fss_socket_request_write(request, &message);
    GNL_MINUS1_CHECK(res, EINVAL, -1)

    gnl_fss_socket_request_destroy(request);

    //TODO: creare gnl_fss_socket_service.c per fare la connessione, la emit, ecc

    printf("message: %s\n", message);

    struct gnl_fss_socket_request *read = gnl_fss_socket_request_read(message);
    printf("Readed message:\n");
    printf("type: %d\n", read->type);
    printf("pathname: %s\n", read->payload.open->pathname);
    printf("flags: %d\n\n", read->payload.open->flags);

    free(message);

    //TODO: codice per la risposta del server

    gnl_fss_socket_request_destroy(read);

    return 0;
}

int gnl_fss_api_read_file(const char *pathname, void **buf, size_t *size) {
    return 0;
}

int gnl_fss_api_read_N_files(int N, const char *dirname) {
    return 0;
}

int gnl_fss_api_write_file(const char *pathname, const char *dirname) {
    return 0;
}

int gnl_fss_api_append_to_file(const char *pathname, void *buf, size_t size, const char *dirname) {
    return 0;
}

int gnl_fss_api_lock_file(const char *pathname) {
    return 0;
}

int gnl_fss_api_unlock_file(const char *pathname) {
    return 0;
}

int gnl_fss_api_close_file(const char *pathname) {
    return 0;
}

int gnl_fss_api_remove_file(const char *pathname) {
    return 0;
}

#include "./macro_end.c"