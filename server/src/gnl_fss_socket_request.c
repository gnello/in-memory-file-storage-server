#include <stdio.h> //TODO: messo per test, rimuoverlo?
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include "../include/gnl_fss_socket_request.h"
#include "./socket/gnl_fss_socket_request_open.c"
#include "./socket/gnl_fss_socket_request_generic.c"
#include "./macro_beg.c"

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

#define GNL_INIT_GENERIC(num, ref, a_list) {                                \
    switch (num) {                                                          \
        case 0:                                                             \
            ref = gnl_fss_socket_request_generic_init();                    \
            break;                                                          \
        case 2:                                                             \
            buffer = va_arg(a_list, char *);                                \
            ref = gnl_fss_socket_request_generic_init_with_args(buffer);    \
            break;                                                          \
        default:                                                            \
        errno = EINVAL;                                                     \
            return NULL;                                                    \
    }                                                                       \
                                                                            \
    GNL_NULL_CHECK(ref, ENOMEM, NULL)                                       \
}

#define GNL_GENERIC_READ_MESSAGE(payload_message, ref, type) {      \
    socket_request = gnl_fss_socket_request_init(type, 0);          \
    GNL_NULL_CHECK(socket_request, ENOMEM, NULL)                    \
                                                                    \
    gnl_fss_socket_request_generic_read(payload_message, ref);      \
}

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

struct gnl_fss_socket_request {
    enum gnl_fss_socket_request_type type;
    union {
        struct gnl_fss_socket_request_open *open;
        struct gnl_fss_socket_request_generic *read;
        struct gnl_fss_socket_read_N *read_n;
        struct gnl_fss_socket_write *write;
        struct gnl_fss_socket_append *append;
        struct gnl_fss_socket_request_generic *lock;
        struct gnl_fss_socket_request_generic *unlock;
        struct gnl_fss_socket_request_generic *close;
        struct gnl_fss_socket_request_generic *remove;
    } payload;
};

/**
 * Calculate the size of the request.
 *
 * @param message   The message of the request.
 *
 * @return          Returns he size of the request on success,
 *                  -1 otherwise.
 */
static int size(const char *message) {
    return MAX_DIGITS_INT + MAX_DIGITS_INT + strlen(message);
}

/**
 * Encode the given socket message and put it into dest.
 *
 * @param message   The message to encode.
 * @param dest      The destination where to put the socket message.
 * @param type      The operation type to encode.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int encode(const char *message, char **dest, enum gnl_fss_socket_request_type type) {
    int request_size = size(message);

    GNL_ALLOCATE_MESSAGE(*dest, request_size + 1)

    int maxlen = request_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*d%0*lu%s", MAX_DIGITS_INT, type, MAX_DIGITS_INT, strlen(message), message);

    return 0;
}

/**
 * Decode the given socket message.
 *
 * @param message   The message to decode.
 * @param dest      The destination where to put the socket message.
 * @param type      The pointer where to put the operation type.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int decode(const char *message, char **dest, enum gnl_fss_socket_request_type *type) {
    size_t message_len;

    // get the operation type and the message length
    sscanf(message, "%"MAX_DIGITS_CHAR"d%"MAX_DIGITS_CHAR"lu", (int *)type, &message_len);

    // allocate memory
    *dest = calloc(message_len + 1, sizeof(char));
    GNL_NULL_CHECK(*dest, ENOMEM, -1)

    // get the message
    strncpy(*dest, message + MAX_DIGITS_INT + MAX_DIGITS_INT, message_len);
    
    return 0;
}

struct gnl_fss_socket_request *gnl_fss_socket_request_init(enum gnl_fss_socket_request_type type, int num, ...) {
    struct gnl_fss_socket_request *socket_request = (struct gnl_fss_socket_request *)malloc(sizeof(struct gnl_fss_socket_request));
    GNL_NULL_CHECK(socket_request, ENOMEM, NULL)

    // initialize valist for num number of arguments
    va_list a_list;
    va_start(a_list, num);

    // assign operation type
    socket_request->type = type;

    // declare utils vars
    char *buffer;

    // assign payload object
    switch (type) {
        case GNL_FSS_SOCKET_REQUEST_OPEN:
            switch (num) {
                case 0:
                    socket_request->payload.open = gnl_fss_socket_request_open_init();
                    break;
                case 2:
                    buffer = va_arg(a_list, char *);
                    socket_request->payload.open = gnl_fss_socket_request_open_init_with_args(buffer, va_arg(a_list, int));
                    break;
                default:
                    errno = EINVAL;
                    return NULL;
            }

            GNL_NULL_CHECK(socket_request->payload.open, ENOMEM, NULL)
            break;

        case GNL_FSS_SOCKET_REQUEST_READ:
            GNL_INIT_GENERIC(num, socket_request->payload.read, a_list)
            break;

        case GNL_FSS_SOCKET_REQUEST_LOCK:
            GNL_INIT_GENERIC(num, socket_request->payload.lock, a_list)
            break;

        case GNL_FSS_SOCKET_REQUEST_UNLOCK:
            GNL_INIT_GENERIC(num, socket_request->payload.unlock, a_list)
            break;

        case GNL_FSS_SOCKET_REQUEST_CLOSE:
            GNL_INIT_GENERIC(num, socket_request->payload.close, a_list)
            break;

        case GNL_FSS_SOCKET_REQUEST_REMOVE:
            GNL_INIT_GENERIC(num, socket_request->payload.remove, a_list)
            break;

        default:
            errno = EINVAL;
            return NULL;
            /* UNREACHED */
            break;
    }

    // clean memory reserved for valist
    va_end(a_list);

    return socket_request;
}

void gnl_fss_socket_request_destroy(struct gnl_fss_socket_request *request) {
    switch (request->type) {
        case GNL_FSS_SOCKET_REQUEST_OPEN:
            gnl_fss_socket_request_open_destroy(request->payload.open);
            break;

        case GNL_FSS_SOCKET_REQUEST_READ:
            gnl_fss_socket_request_generic_destroy(request->payload.read);
            break;

        case GNL_FSS_SOCKET_REQUEST_LOCK:
            gnl_fss_socket_request_generic_destroy(request->payload.lock);
            break;

        case GNL_FSS_SOCKET_REQUEST_UNLOCK:
            gnl_fss_socket_request_generic_destroy(request->payload.unlock);
            break;

        case GNL_FSS_SOCKET_REQUEST_CLOSE:
            gnl_fss_socket_request_generic_destroy(request->payload.close);
            break;

        case GNL_FSS_SOCKET_REQUEST_REMOVE:
            gnl_fss_socket_request_generic_destroy(request->payload.remove);
            break;
    }

    free(request);
}

struct gnl_fss_socket_request *gnl_fss_socket_request_read(const char *message) {
    struct gnl_fss_socket_request *socket_request;

    char *payload_message;
    enum gnl_fss_socket_request_type type;

    decode(message, &payload_message, &type);

    switch (type) {
        case GNL_FSS_SOCKET_REQUEST_OPEN:
            socket_request = gnl_fss_socket_request_init(GNL_FSS_SOCKET_REQUEST_OPEN, 0);
            GNL_NULL_CHECK(socket_request, ENOMEM, NULL)

            gnl_fss_socket_request_open_read(payload_message, socket_request->payload.open);
            break;

        case GNL_FSS_SOCKET_REQUEST_READ:
            GNL_GENERIC_READ_MESSAGE(payload_message, socket_request->payload.read, type);
            break;

        case GNL_FSS_SOCKET_REQUEST_LOCK:
            GNL_GENERIC_READ_MESSAGE(payload_message, socket_request->payload.lock, type);
            break;

        case GNL_FSS_SOCKET_REQUEST_UNLOCK:
            GNL_GENERIC_READ_MESSAGE(payload_message, socket_request->payload.unlock, type);
            break;

        case GNL_FSS_SOCKET_REQUEST_CLOSE:
            GNL_GENERIC_READ_MESSAGE(payload_message, socket_request->payload.close, type);
            break;

        case GNL_FSS_SOCKET_REQUEST_REMOVE:
            GNL_GENERIC_READ_MESSAGE(payload_message, socket_request->payload.remove, type);
            break;

        default:
            errno = EINVAL;
            return NULL;
            /* UNREACHED */
            break;
    }

    free(payload_message);

    return socket_request;
}

int gnl_fss_socket_request_write(gnl_fss_socket_request *request, char **dest) {
    char *built_message;

    switch (request->type) {
        case GNL_FSS_SOCKET_REQUEST_OPEN:
            gnl_fss_socket_request_open_write(*(request->payload.open), &built_message);
            break;

        case GNL_FSS_SOCKET_REQUEST_READ:
            gnl_fss_socket_request_generic_write(*(request->payload.read), &built_message);
            break;

        case GNL_FSS_SOCKET_REQUEST_LOCK:
            gnl_fss_socket_request_generic_write(*(request->payload.lock), &built_message);
            break;

        case GNL_FSS_SOCKET_REQUEST_UNLOCK:
            gnl_fss_socket_request_generic_write(*(request->payload.unlock), &built_message);
            break;

        case GNL_FSS_SOCKET_REQUEST_CLOSE:
            gnl_fss_socket_request_generic_write(*(request->payload.close), &built_message);
            break;

        case GNL_FSS_SOCKET_REQUEST_REMOVE:
            gnl_fss_socket_request_generic_write(*(request->payload.remove), &built_message);
            break;

        default:
            errno = EINVAL;
            return -1;
            /* UNREACHED */
            break;
    }

    encode(built_message, dest, request->type);

    free(built_message);

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include "./macro_end.c"