#include <stdio.h> //TODO: messo per test, rimuoverlo?
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "./socket/gnl_fss_socket_open.c"
#include "./socket/macro_beg.c"

enum gnl_fss_socket_op {
    GNL_FSS_SOCKET_OP_OPEN
};

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
    enum gnl_fss_socket_op type;
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

/**
 *
 * @param message
 * @param dest
 * @param op
 * @return
 */
static int attach_metadata(const char *message, char **dest, enum gnl_fss_socket_op op) {
    GNL_ALLOCATE_MESSAGE(*dest, sizeof(op) + strlen(message) + 1)

    sprintf(*dest, "%d %lu %s", op, strlen(message), message);

    return 0;
}

/**
 *
 * @param message
 * @param dest
 * @return
 */
static int prepare_message(struct gnl_fss_socket_message message, char **dest) {
    char *buffer;

    switch (message.type) {
        case GNL_FSS_SOCKET_OP_OPEN:
            gnl_fss_socket_open_prepare_message(message.payload.open, &buffer);
            attach_metadata(buffer, dest, GNL_FSS_SOCKET_OP_OPEN);
            break;

        default:
            errno = EINVAL;
            return -1;
            /* UNREACHED */
            break;
    }

    free(buffer);

    return -1;
}

/**
 *
 * @param message
 * @param dest
 * @return
 */
struct gnl_fss_socket_message *gnl_fss_socket_read_message(const char *message) {
    struct gnl_fss_socket_message *socket_message = (struct gnl_fss_socket_message *)malloc(sizeof(struct gnl_fss_socket_message));
    GNL_NULL_CHECK(socket_message, ENOMEM, NULL)

    size_t message_len;

    enum gnl_fss_socket_op op;
    sscanf(message, "%d %lu", &socket_message->type, &message_len);

    char mex[message_len];
    sscanf(message, "%d %lu %s", &socket_message->type, &message_len, mex);

    printf("TYPE: %d, MEX_LEN: %lu, MEX: %s\n", socket_message->type, message_len, mex);

    return socket_message;
}

int gnl_fss_socket_send(struct gnl_fss_socket_message message) {
    char *message2 = NULL;

    prepare_message(message, &message2);

    printf("%s\n", message2);

    free(message2);

    return 0;
}

#include "./socket/macro_end.c"