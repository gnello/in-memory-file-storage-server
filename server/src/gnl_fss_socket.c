#include <stdio.h> //TODO: messo per test, rimuoverlo?
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include "./socket/gnl_fss_socket_open.c"
#include "./socket/gnl_fss_socket_generic.c"
#include "./socket/macro_beg.c"

enum gnl_fss_socket_op {
    GNL_FSS_SOCKET_OP_OPEN,
    GNL_FSS_SOCKET_OP_READ
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

struct gnl_fss_socket_message {
    enum gnl_fss_socket_op type;
    union {
        struct gnl_fss_socket_open *open;
        struct gnl_fss_socket_generic *read;
        struct gnl_fss_socket_read_N *read_n;
        struct gnl_fss_socket_write *write;
        struct gnl_fss_socket_append *append;
        struct gnl_fss_socket_generic *lock;
        struct gnl_fss_socket_generic *unlock;
        struct gnl_fss_socket_generic *close;
        struct gnl_fss_socket_generic *remove;
    } payload;
};

/**
 * Encode the given socket message and put it into dest.
 *
 * @param message   The message to encode.
 * @param dest      The destination where to put the socket message.
 * @param op        The operation type to encode.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int encode(const char *message, char **dest, enum gnl_fss_socket_op op) {
    GNL_ALLOCATE_MESSAGE(*dest, sizeof(op) + sizeof(unsigned long) + ((strlen(message) + 1) * sizeof(char)))

    sprintf(*dest, "%d %lu %s", op, strlen(message), message);

    return 0;
}

/**
 * Decode the given socket message.
 *
 * @param message   The message to decode.
 * @param dest      The destination where to put the socket message.
 * @param op        The pointer where to put the operation type.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int decode(const char *message, char **dest, enum gnl_fss_socket_op *op) {
    size_t message_len;

    // get the operation type and the message length
    sscanf(message, "%d %lu", (int *)op, &message_len);

    // allocate memory
    *dest = malloc((message_len + 1) * sizeof(char));
    GNL_NULL_CHECK(*dest, ENOMEM, -1)

    // get the socket message
    sscanf(message, "%d %lu %s", (int *)op, &message_len, *dest);

    return 0;
}

/**
 *
 * @param op
 * @param num
 * @param ...
 * @return
 */
struct gnl_fss_socket_message *gnl_fss_socket_message_init(enum gnl_fss_socket_op op, int num, ...) {
    struct gnl_fss_socket_message *socket_message = (struct gnl_fss_socket_message *)malloc(sizeof(struct gnl_fss_socket_message));
    GNL_NULL_CHECK(socket_message, ENOMEM, NULL)

    // initialize valist for num number of arguments
    va_list a_list;
    va_start(a_list, num);

    // assign operation type
    socket_message->type = op;

    // declare utils vars
    char *buffer;

    // assign payload object
    switch (op) {
        case GNL_FSS_SOCKET_OP_OPEN:
            switch (num) {
                case 0:
                    socket_message->payload.open = gnl_fss_socket_open_init();
                    break;
                case 2:
                    buffer = va_arg(a_list, char *);
                    socket_message->payload.open = gnl_fss_socket_open_init_with_args(buffer, va_arg(a_list, int));
                    break;
                default:
                    errno = EINVAL;
                    return NULL;
            }

            GNL_NULL_CHECK(socket_message->payload.open, ENOMEM, NULL)
            break;

        case GNL_FSS_SOCKET_OP_READ:
            switch (num) {
                case 0:
                    socket_message->payload.read = gnl_fss_socket_generic_init();
                    break;
                case 2:
                    buffer = va_arg(a_list, char *);
                    socket_message->payload.read = gnl_fss_socket_generic_init_with_args(buffer);
                    break;
                default:
                    errno = EINVAL;
                    return NULL;
            }

            GNL_NULL_CHECK(socket_message->payload.read, ENOMEM, NULL)
            break;

        default:
            errno = EINVAL;
            return NULL;
            /* UNREACHED */
            break;
    }

    // clean memory reserved for valist
    va_end(a_list);

    return socket_message;
}

/**
 *
 * @param message
 */
void gnl_fss_socket_message_destroy(struct gnl_fss_socket_message *message) {
    switch (message->type) {
        case GNL_FSS_SOCKET_OP_OPEN:
            gnl_fss_socket_open_destroy(message->payload.open);
            break;

        case GNL_FSS_SOCKET_OP_READ:
            gnl_fss_socket_generic_destroy(message->payload.read);
            break;
    }

    free(message);
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
            gnl_fss_socket_open_build_message(*message.payload.open, &buffer);
            encode(buffer, dest, GNL_FSS_SOCKET_OP_OPEN);
            break;

        case GNL_FSS_SOCKET_OP_READ:
            gnl_fss_socket_generic_build_message(*message.payload.read, &buffer);
            encode(buffer, dest, GNL_FSS_SOCKET_OP_READ);
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
    struct gnl_fss_socket_message *socket_message;

    char *payload_message;
    enum gnl_fss_socket_op op;

    decode(message, &payload_message, &op);

    switch (op) {
        case GNL_FSS_SOCKET_OP_OPEN:
            socket_message = gnl_fss_socket_message_init(GNL_FSS_SOCKET_OP_OPEN, 0);
            GNL_NULL_CHECK(socket_message, ENOMEM, NULL)

            gnl_fss_socket_open_read_message(payload_message, socket_message->payload.open);
            break;

        case GNL_FSS_SOCKET_OP_READ:
            socket_message = gnl_fss_socket_message_init(GNL_FSS_SOCKET_OP_READ, 0);
            GNL_NULL_CHECK(socket_message, ENOMEM, NULL)

            gnl_fss_socket_generic_read_message(payload_message, socket_message->payload.read);
            break;

        default:
            errno = EINVAL;
            return NULL;
            /* UNREACHED */
            break;
    }

    //printf("TYPE: %d, MEX: %s\n", socket_message->type, buffer);
    printf("PATHNAME: %s, FLAGS: %d\n\n", socket_message->payload.open->pathname, socket_message->payload.open->flags);

    free(payload_message);

    return socket_message;
}

int gnl_fss_socket_send(struct gnl_fss_socket_message message) {
    char *message2 = NULL;

    prepare_message(message, &message2);

    printf("sending message: %s\n", message2);

    free(message2);

    return 0;
}

#include "./socket/macro_end.c"