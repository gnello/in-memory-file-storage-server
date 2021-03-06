#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <gnl_message_s.h>
#include <gnl_message_n.h>
#include <gnl_message_sn.h>
#include <gnl_message_nnb.h>
#include <gnl_macro_beg.h>
#include "../include/gnl_socket_request.h"

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

#define GNL_REQUEST_N_INIT(num, ref, a_list) {                          \
    switch (num) {                                                      \
        case 0:                                                         \
            ref = gnl_message_n_init();                                 \
            break;                                                      \
        case 1:                                                         \
            ref = gnl_message_n_init_with_args(va_arg(a_list, int));    \
            break;                                                      \
        default:                                                        \
            errno = EINVAL;                                             \
            return NULL;                                                \
    }                                                                   \
                                                                        \
    GNL_NULL_CHECK(ref, ENOMEM, NULL)                                   \
}

#define GNL_REQUEST_S_INIT(num, ref, a_list) {                              \
    switch (num) {                                                          \
        case 0:                                                             \
            ref = gnl_message_s_init();                                     \
            break;                                                          \
        case 1:                                                             \
            buffer_s = va_arg(a_list, char *);                              \
            ref = gnl_message_s_init_with_args(buffer_s);                   \
            break;                                                          \
        default:                                                            \
            errno = EINVAL;                                                 \
            return NULL;                                                    \
    }                                                                       \
                                                                            \
    GNL_NULL_CHECK(ref, ENOMEM, NULL)                                       \
}

#define GNL_REQUEST_SNB_INIT(num, ref, a_list) {                                                         \
    switch (num) {                                                                                      \
        case 0:                                                                                         \
            ref = gnl_message_snb_init();                               \
            break;                                                                                      \
        case 3:                                                                                         \
            buffer_s = va_arg(a_list, char *);                                                          \
            buffer_n1 = va_arg(a_list, int);                                                            \
            buffer_b = va_arg(a_list, void *);                                                          \
            ref = gnl_message_snb_init_with_args(buffer_s, buffer_n1, buffer_b);   \
        break;                                                                                          \
            default:                                                                                    \
            errno = EINVAL;                                                                             \
            return NULL;                                                                                \
    }                                                                                                   \
                                                                                                        \
    GNL_NULL_CHECK(ref, ENOMEM, NULL)                                       \
}

#define GNL_REQUEST_NNB_INIT(num, ref, a_list) {                                                         \
    switch (num) {                                                                                      \
        case 0:                                                                                         \
            ref = gnl_message_nnb_init();                                                                \
            break;                                                                                      \
        case 3:                                                                                         \
            buffer_n1 = va_arg(a_list, int);                                                            \
            buffer_n2 = va_arg(a_list, size_t);                                                         \
            buffer_b = va_arg(a_list, void *);                                                          \
            ref = gnl_message_nnb_init_with_args(buffer_n1, buffer_n2, buffer_b);                        \
        break;                                                                                          \
            default:                                                                                    \
            errno = EINVAL;                                                                             \
            return NULL;                                                                                \
    }                                                                                                   \
                                                                                                        \
    GNL_NULL_CHECK(ref, ENOMEM, NULL)                                                                   \
}

#define GNL_REQUEST_N_READ_MESSAGE(payload_message, ref, type) {    \
    request = gnl_socket_request_init(type, 0);                     \
    GNL_NULL_CHECK(request, ENOMEM, NULL)                           \
                                                                    \
    gnl_message_n_from_string(payload_message, ref);                \
}

#define GNL_REQUEST_S_READ_MESSAGE(payload_message, ref, type) {    \
    request = gnl_socket_request_init(type, 0);                     \
    GNL_NULL_CHECK(request, ENOMEM, NULL)                           \
                                                                    \
    gnl_message_s_from_string(payload_message, ref);                \
}

#define GNL_REQUEST_SNB_READ_MESSAGE(payload_message, ref, type) {   \
    request = gnl_socket_request_init(type, 0);                     \
    GNL_NULL_CHECK(request, ENOMEM, NULL)                           \
                                                                    \
    gnl_message_snb_from_string(payload_message, ref);               \
}

#define GNL_REQUEST_NNB_READ_MESSAGE(payload_message, ref, type) {  \
    request = gnl_socket_request_init(type, 0);                     \
    GNL_NULL_CHECK(request, ENOMEM, NULL)                           \
                                                                    \
    gnl_message_nnb_from_string(payload_message, ref);              \
}

/**
 * {@inheritDoc}
 */
struct gnl_socket_request {
    enum gnl_socket_request_type type;
    union {
        struct gnl_message_sn *open;
        struct gnl_message_n *read;
        struct gnl_message_n *read_N;
        struct gnl_message_nnb *write;
        struct gnl_message_n *lock;
        struct gnl_message_n *unlock;
        struct gnl_message_n *close;
        struct gnl_message_s *remove;
    } payload;
};

/**
 * {@inheritDoc}
 */
int gnl_socket_request_get_type(const struct gnl_socket_request *request, char **dest) {
    if (request == NULL) {
        errno = EINVAL;

        return -1;
    }

    switch (request->type) {
        case GNL_SOCKET_REQUEST_OPEN:
        GNL_CALLOC(*dest, 5, -1);
            strcpy(*dest, "OPEN");
            break;

        case GNL_SOCKET_REQUEST_READ_N:
        GNL_CALLOC(*dest, 7, -1);
            strcpy(*dest, "READ_N");
            break;

        case GNL_SOCKET_REQUEST_READ:
        GNL_CALLOC(*dest, 5, -1);
            strcpy(*dest, "READ");
            break;

        case GNL_SOCKET_REQUEST_WRITE:
        GNL_CALLOC(*dest, 6, -1);
            strcpy(*dest, "WRITE");
            break;

        case GNL_SOCKET_REQUEST_LOCK:
        GNL_CALLOC(*dest, 5, -1);
            strcpy(*dest, "LOCK");
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
        GNL_CALLOC(*dest, 7, -1);
            strcpy(*dest, "UNLOCK");
            break;

        case GNL_SOCKET_REQUEST_CLOSE:
        GNL_CALLOC(*dest, 6, -1);
            strcpy(*dest, "CLOSE");
            break;

        case GNL_SOCKET_REQUEST_REMOVE:
        GNL_CALLOC(*dest, 7, -1);
            strcpy(*dest, "REMOVE");
            break;

        default:
            errno = EINVAL;
            return -1;
            /* UNREACHED */
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
struct gnl_socket_request *gnl_socket_request_init(enum gnl_socket_request_type type, int num, ...) {
    struct gnl_socket_request *socket_request = (struct gnl_socket_request *)malloc(sizeof(struct gnl_socket_request));
    GNL_NULL_CHECK(socket_request, ENOMEM, NULL)

    // initialize valist for num number of arguments
    va_list a_list;
    va_start(a_list, num);

    // assign operation type
    socket_request->type = type;

    // declare buffer vars
    char *buffer_s;
    char *buffer_b;
    int buffer_n1;
    int buffer_n2;

    // assign payload object
    switch (type) {
        case GNL_SOCKET_REQUEST_OPEN:
            switch (num) {
                case 0:
                    socket_request->payload.open = gnl_message_sn_init();
                    break;
                case 2:
                    buffer_s = va_arg(a_list, char *);
                    socket_request->payload.open = gnl_message_sn_init_with_args(buffer_s, va_arg(a_list, int));
                    break;
                default:
                    errno = EINVAL;
                    return NULL;
            }

            GNL_NULL_CHECK(socket_request->payload.open, ENOMEM, NULL)
            break;

        case GNL_SOCKET_REQUEST_READ_N:
            GNL_REQUEST_N_INIT(num, socket_request->payload.read_N, a_list)
            break;

        case GNL_SOCKET_REQUEST_READ:
            GNL_REQUEST_N_INIT(num, socket_request->payload.read, a_list)
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            GNL_REQUEST_NNB_INIT(num, socket_request->payload.write, a_list)
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            GNL_REQUEST_N_INIT(num, socket_request->payload.lock, a_list)
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            GNL_REQUEST_N_INIT(num, socket_request->payload.unlock, a_list)
            break;

        case GNL_SOCKET_REQUEST_CLOSE:
            GNL_REQUEST_N_INIT(num, socket_request->payload.close, a_list)
            break;

        case GNL_SOCKET_REQUEST_REMOVE:
            GNL_REQUEST_S_INIT(num, socket_request->payload.remove, a_list)
            break;

        default:
            errno = EINVAL;
            return NULL;
            /* UNREACHED */
    }

    // clean memory reserved for valist
    va_end(a_list);

    return socket_request;
}

/**
 * {@inheritDoc}
 */
void gnl_socket_request_destroy(struct gnl_socket_request *request) {
    if (request == NULL) {
        return;
    }

    switch (request->type) {
        case GNL_SOCKET_REQUEST_OPEN:
            gnl_message_sn_destroy(request->payload.open);
            break;

        case GNL_SOCKET_REQUEST_READ_N:
            gnl_message_n_destroy(request->payload.read_N);
            break;

        case GNL_SOCKET_REQUEST_READ:
            gnl_message_n_destroy(request->payload.read);
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            gnl_message_nnb_destroy(request->payload.write);
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            gnl_message_n_destroy(request->payload.lock);
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            gnl_message_n_destroy(request->payload.unlock);
            break;

        case GNL_SOCKET_REQUEST_CLOSE:
            gnl_message_n_destroy(request->payload.close);
            break;

        case GNL_SOCKET_REQUEST_REMOVE:
            gnl_message_s_destroy(request->payload.remove);
            break;
    }

    free(request);
}

/**
 * Build a request from the given string.
 *
 * @param message   The message from where to build the request.
 * @param type      The type of the request.
 * @param request   The pointer where to build the request.
 *
 * @return          Returns the built gnl_socket_request on success,
 *                  -1 otherwise.
 */
struct gnl_socket_request *gnl_socket_request_from_string(const char *message, enum gnl_socket_request_type type) {
    //validate the parameters
    GNL_NULL_CHECK(message, EINVAL, NULL)

    struct gnl_socket_request *request;

    // get the request string
    switch (type) {
        case GNL_SOCKET_REQUEST_OPEN:
            request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 0);
            GNL_NULL_CHECK(request, ENOMEM, NULL)

            gnl_message_sn_from_string(message, request->payload.open);
            break;

        case GNL_SOCKET_REQUEST_READ_N:
            GNL_REQUEST_N_READ_MESSAGE(message, request->payload.read_N, type);
            break;

        case GNL_SOCKET_REQUEST_READ:
            GNL_REQUEST_N_READ_MESSAGE(message, request->payload.read, type);
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            GNL_REQUEST_NNB_READ_MESSAGE(message, request->payload.write, type);
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            GNL_REQUEST_N_READ_MESSAGE(message, request->payload.lock, type);
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            GNL_REQUEST_N_READ_MESSAGE(message, request->payload.unlock, type);
            break;

        case GNL_SOCKET_REQUEST_CLOSE:
            GNL_REQUEST_N_READ_MESSAGE(message, request->payload.close, type);
            break;

        case GNL_SOCKET_REQUEST_REMOVE:
            GNL_REQUEST_S_READ_MESSAGE(message, request->payload.remove, type);
            break;

        default:
            errno = EINVAL;
            return NULL;
            /* UNREACHED */
    }

    return request;
}

/**
 * {@inheritDoc}
 */
size_t gnl_socket_request_to_string(const struct gnl_socket_request *request, char **dest) {

    // validate the parameters
    GNL_NULL_CHECK(request, EINVAL, -1)
    GNL_MINUS1_CHECK(-1 * (*dest != NULL), EINVAL, -1)

    size_t message_len;

    // get the request string
    switch (request->type) {
        case GNL_SOCKET_REQUEST_OPEN:
            message_len = gnl_message_sn_to_string(request->payload.open, dest);
            break;

        case GNL_SOCKET_REQUEST_READ_N:
            message_len = gnl_message_n_to_string(request->payload.read_N, dest);
            break;

        case GNL_SOCKET_REQUEST_READ:
            message_len = gnl_message_n_to_string(request->payload.read, dest);
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            message_len = gnl_message_nnb_to_string(request->payload.write, dest);
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            message_len = gnl_message_n_to_string(request->payload.lock, dest);
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            message_len = gnl_message_n_to_string(request->payload.unlock, dest);
            break;

        case GNL_SOCKET_REQUEST_CLOSE:
            message_len = gnl_message_n_to_string(request->payload.close, dest);
            break;

        case GNL_SOCKET_REQUEST_REMOVE:
            message_len = gnl_message_s_to_string(request->payload.remove, dest);
            break;

        default:
            errno = EINVAL;
            return -1;
            /* UNREACHED */
    }

    // check the message_len
    if (message_len <= 0) {
        free(*dest);

        return -1;
    }

    return message_len;
}

/**
 * {@inheritDoc}
 */
int gnl_socket_request_type(const struct gnl_socket_request *request) {
    GNL_NULL_CHECK(request, EINVAL, -1)

    return request->type;
}

/**
 * {@inheritDoc}
 */
int gnl_socket_request_get_fd(const struct gnl_socket_request *request) {
    GNL_NULL_CHECK(request, EINVAL, -1)

    int fd = -1;

    switch (request->type) {

        case GNL_SOCKET_REQUEST_READ:
            fd = request->payload.read->number;
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            fd = request->payload.write->number;
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            fd = request->payload.lock->number;
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            fd = request->payload.unlock->number;
            break;

        case GNL_SOCKET_REQUEST_CLOSE:
            fd = request->payload.close->number;
            break;

        default:
            errno = EINVAL;
            break;
    }

    return fd;
}

/**
 * {@inheritDoc}
 */
char *gnl_socket_request_get_filename(const struct gnl_socket_request *request) {
    GNL_NULL_CHECK(request, EINVAL, NULL)

    char *filename = NULL;

    switch (request->type) {

        case GNL_SOCKET_REQUEST_OPEN:
            filename = request->payload.open->string;
            break;

        case GNL_SOCKET_REQUEST_REMOVE:
            filename = request->payload.remove->string;
            break;

        default:
            errno = EINVAL;
            break;
    }

    return filename;
}

/**
 * {@inheritDoc}
 */
int gnl_socket_request_get_flags(const struct gnl_socket_request *request) {
    GNL_NULL_CHECK(request, EINVAL, -1)

    int flags = 0;

    switch (request->type) {

        case GNL_SOCKET_REQUEST_OPEN:
            flags = request->payload.open->number;
            break;

        default:
            errno = EINVAL;
            break;
    }

    return flags;
}

/**
 * {@inheritDoc}
 */
size_t gnl_socket_request_get_size(const struct gnl_socket_request *request) {
    GNL_NULL_CHECK(request, EINVAL, -1)

    if (request->type != GNL_SOCKET_REQUEST_WRITE) {
        errno = EINVAL;

        return -1;
    }

    return request->payload.write->count;
}

/**
 * {@inheritDoc}
 */
void *gnl_socket_request_get_bytes(const struct gnl_socket_request *request) {
    GNL_NULL_CHECK(request, EINVAL, NULL)

    if (request->type != GNL_SOCKET_REQUEST_WRITE) {
        errno = EINVAL;

        return NULL;
    }

    return request->payload.write->bytes;
}

#undef MAX_DIGITS_CHAR
#undef MAX_DIGITS_INT
#undef GNL_REQUEST_N_INIT
#undef GNL_REQUEST_S_INIT
#undef GNL_REQUEST_SNB_INIT
#undef GNL_REQUEST_NNB_INIT
#undef GNL_REQUEST_N_READ_MESSAGE
#undef GNL_REQUEST_S_READ_MESSAGE
#undef GNL_REQUEST_SNB_READ_MESSAGE
#undef GNL_REQUEST_NNB_READ_MESSAGE

#include <gnl_macro_end.h>