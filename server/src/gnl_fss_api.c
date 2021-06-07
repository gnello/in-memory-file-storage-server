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
    struct gnl_fss_socket_message *message = gnl_fss_socket_message_init(GNL_FSS_SOCKET_OP_OPEN);
    //TODO: check null
    message->payload.open->pathname = malloc((strlen(pathname) + 1) * sizeof(char));
    //TODO: check null

    strcpy(message->payload.open->pathname, pathname);
    message->payload.open->flags = flags;

    gnl_fss_socket_send(*message);

    struct gnl_fss_socket_message *msg;

    msg = gnl_fss_socket_read_message("0 30 0000000019./pino/il/ginepraio3");
    gnl_fss_socket_message_destroy(msg);

    msg = gnl_fss_socket_read_message("0 29 0000000018./pino/il/bischero0");
    gnl_fss_socket_message_destroy(msg);

    msg = gnl_fss_socket_read_message("0 31 0000000020./pino/il/panettiere1");
    gnl_fss_socket_message_destroy(msg);

    msg = gnl_fss_socket_read_message("0 31 0000000020./pino/il/gattopardo2");
    gnl_fss_socket_message_destroy(msg);

    gnl_fss_socket_message_destroy(message);

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