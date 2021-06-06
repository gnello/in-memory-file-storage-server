#define _POSIX_C_SOURCE 199309L

#include "../include/gnl_fss_api.h"
#include "./gnl_fss_socket.c" //TODO: far diventare libreria statica?

int gnl_fss_api_open_connection(const char *sockname, int msec, const struct timespec abstime) {
    return 0;
}

int gnl_fss_api_close_connection(const char *sockname) {
    return 0;
}

int gnl_fss_api_open_file(const char *pathname, int flags) {
    struct gnl_fss_socket_message message;

    message.type = GNL_FSS_SOCKET_OP_OPEN;
    message.payload.open.pathname = pathname;
    message.payload.open.flags = flags;

    gnl_fss_socket_send(message);

    gnl_fss_socket_read_message("0 20./pino/il/gattopardo2");

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