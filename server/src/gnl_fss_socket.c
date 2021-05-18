#include <stdio.h> //TODO: messo per test, rimuoverlo?
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "./socket/gnl_fss_socket_open.c"

const int GNL_FSS_SOCKET_READ = 2;
const int GNL_FSS_SOCKET_WRITE = 3;

#define GNL_ALLOCATE_MESSAGE(ptr, len) {        \
    ptr = (char *)malloc(len * sizeof(char));   \
    if (ptr == NULL) {                          \
        errno = ENOMEM;                         \
                                                \
        return NULL;                            \
    }                                           \
}

struct gnl_fss_socket_read {
    const char *pathname;
    void **buf;
    size_t *size;
};

struct gnl_fss_socket_read_N {
    int N;
    const char *dirname;
};

struct gnl_fss_socket_write {
    const char *pathname;
    const char *dirname;
};

struct gnl_fss_socket_append {
    const char *pathname;
    void *buf;
    size_t size;
    const char *dirname;
};

struct gnl_fss_socket_generic {
    const char *pathname;
};

struct gnl_fss_socket_message {
    int type;
    union {
        struct gnl_fss_socket_open open;
        struct gnl_fss_socket_read read;
        struct gnl_fss_socket_read_N read_n;
        struct gnl_fss_socket_write write;
        struct gnl_fss_socket_append append;
        struct gnl_fss_socket_generic lock;
        struct gnl_fss_socket_generic unlock;
        struct gnl_fss_socket_generic close;
        struct gnl_fss_socket_generic remove;
    } payload;
};

static char *prepare_message(struct gnl_fss_socket_message message) {
    char *final_message;
    int pathname_len;

    switch (message.type) {
        case 1:
            pathname_len = strlen(message.payload.open.pathname);
            GNL_ALLOCATE_MESSAGE(final_message, 3 + sizeof(pathname_len) + gnl_fss_socket_open_size(message.payload.open) + 1)

            sprintf(final_message, "OPN%d%s", pathname_len, message.payload.open.pathname);
            break;

        default:
            errno = EINVAL;
            return NULL;
            /* UNREACHED */
            break;
    }

    return final_message;
}

int gnl_fss_socket_send(struct gnl_fss_socket_message message) {
    char *message2 = prepare_message(message);

    printf("%s\n", message2);

    free(message2);

    return 0;
}

#undef GNL_ALLOCATE_MESSAGE