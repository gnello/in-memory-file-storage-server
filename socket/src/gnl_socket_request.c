#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <gnl_message_s.h>
#include <gnl_message_n.h>
#include <gnl_message_sn.h>
#include <gnl_message_sb.h>
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

#define GNL_REQUEST_SB_INIT(num, ref, a_list) {                                                         \
    switch (num) {                                                                                      \
        case 0:                                                                                         \
            ref = gnl_message_sb_init();                               \
            break;                                                                                      \
        case 2:                                                                                         \
            buffer_s = va_arg(a_list, char *);                                                          \
            buffer_b = va_arg(a_list, void *);                                                          \
            ref = gnl_message_sb_init_with_args(buffer_s, buffer_b);   \
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
        case 2:                                                                                         \
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
    *request = gnl_socket_request_init(type, 0);                    \
    GNL_NULL_CHECK(*request, ENOMEM, -1)                            \
                                                                    \
    gnl_message_n_read(payload_message, ref);                       \
}

#define GNL_REQUEST_S_READ_MESSAGE(payload_message, ref, type) {    \
    *request = gnl_socket_request_init(type, 0);                    \
    GNL_NULL_CHECK(*request, ENOMEM, -1)                            \
                                                                    \
    gnl_message_s_read(payload_message, ref);                       \
}

#define GNL_REQUEST_SB_READ_MESSAGE(payload_message, ref, type) {   \
    *request = gnl_socket_request_init(type, 0);                    \
    GNL_NULL_CHECK(*request, ENOMEM, -1)                            \
                                                                    \
    gnl_message_sb_read(payload_message, ref);                      \
}

#define GNL_REQUEST_NNB_READ_MESSAGE(payload_message, ref, type) {  \
    *request = gnl_socket_request_init(type, 0);                    \
    GNL_NULL_CHECK(*request, ENOMEM, -1)                            \
                                                                    \
    gnl_message_nnb_read(payload_message, ref);                     \
}

/**
 * Calculate the size of the request.
 *
 * @return          Returns he size of the request on success,
 *                  -1 otherwise.
 */
static int size() {
    return MAX_DIGITS_INT + MAX_DIGITS_INT;
}

/**
 * Encode the given socket message and put it into dest.
 *
 * @param message   The message to encode.
 * @param dest      The destination where to put the socket message.
 * @param count     The number of bytes of the message.
 * @param type      The operation type to encode.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int encode(const char *message, char **dest, size_t count, enum gnl_socket_request_type type) {
    int request_size = size();

    GNL_CALLOC(*dest, request_size + 1 + count, -1)

    int maxlen = request_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*d%0*lu", MAX_DIGITS_INT, type, MAX_DIGITS_INT, count);

    memcpy(*dest + maxlen, message, count);

    return maxlen + count;
}
ssize_t  /* Read "n" bytes from a descriptor */
readn(int fd, void *ptr, size_t n) {
    size_t   nleft;
    ssize_t  nread;

    nleft = n;
    while (nleft > 0) {
        if((nread = read(fd, ptr, nleft)) < 0) {
            if (nleft == n) return -1; /* error, return -1 */
            else break; /* error, return amount read so far */
        } else if (nread == 0) break; /* EOF */
        nleft -= nread;
        ptr   += nread;
    }
    return(n - nleft); /* return >= 0 */
}
/**
 * Decode the given socket message.
 *
 * @param fd        The file descriptor where to read.
 * @param dest      The destination where to put the socket message.
 * @param type      The pointer where to put the operation type.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static size_t decode(int fd, char **dest, enum gnl_socket_request_type *type) {
    char *message;
    size_t message_len;

    // allocate memory for the initial message reading
    GNL_CALLOC(message, 21, -1)

    // read the first 21 chars, the protocol standard puts
    // the type of the message in the first 10 chars and the
    // size of the message in the second 10 chars, the remaining
    // byte is the null terminator char
    size_t nread = readn(fd, message, 21);
    GNL_MINUS1_CHECK(nread, errno, -1)

    // if nread == 0 the connection is closed, bubble it
    if (nread == 0) {
        return 0;
    }

    // check if the read succeeded
    if (nread != 21) {
        errno = EBADMSG;
        return -1;
    }

    // get the operation type and the message length
    sscanf(message, "%"MAX_DIGITS_CHAR"d%"MAX_DIGITS_CHAR"lu", (int *)type, &message_len);

    // free memory
    free(message);

    // allocate memory for the payload message
    GNL_CALLOC(*dest, message_len, -1)

    // get the message
    nread = readn(fd, *dest, message_len);
    GNL_MINUS1_CHECK(nread, errno, -1)

    // check if the read succeeded
    if (nread != message_len) {
        errno = EBADMSG;
        return -1;
    }

    return nread;
}

/**
 * {@inheritDoc}
 */
int gnl_socket_request_to_string(struct gnl_socket_request *request, char **dest) {
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

        case GNL_SOCKET_REQUEST_APPEND:
        GNL_CALLOC(*dest, 7, -1);
            strcpy(*dest, "APPEND");
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
            break;
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
            GNL_REQUEST_S_INIT(num, socket_request->payload.read, a_list)
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            GNL_REQUEST_NNB_INIT(num, socket_request->payload.write, a_list)
            break;

        case GNL_SOCKET_REQUEST_APPEND:
            GNL_REQUEST_SB_INIT(num, socket_request->payload.append, a_list)
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            GNL_REQUEST_S_INIT(num, socket_request->payload.lock, a_list)
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            GNL_REQUEST_S_INIT(num, socket_request->payload.unlock, a_list)
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
            break;
    }

    // clean memory reserved for valist
    va_end(a_list);

    return socket_request;
}

/**
 * {@inheritDoc}
 */
void gnl_socket_request_destroy(struct gnl_socket_request *request) {
    switch (request->type) {
        case GNL_SOCKET_REQUEST_OPEN:
            gnl_message_sn_destroy(request->payload.open);
            break;

        case GNL_SOCKET_REQUEST_READ_N:
            gnl_message_n_destroy(request->payload.read_N);
            break;

        case GNL_SOCKET_REQUEST_READ:
            gnl_message_s_destroy(request->payload.read);
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            gnl_message_nnb_destroy(request->payload.write);
            break;

        case GNL_SOCKET_REQUEST_APPEND:
            gnl_message_sb_destroy(request->payload.append);
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            gnl_message_s_destroy(request->payload.lock);
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            gnl_message_s_destroy(request->payload.unlock);
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
 * {@inheritDoc}
 */
size_t gnl_socket_request_read(int fd, struct gnl_socket_request **request) {
    if (*request != NULL) {
        errno = EINVAL;

        return -1;
    }

    char *payload_message;
    enum gnl_socket_request_type type;

    int nread = decode(fd, &payload_message, &type);
    GNL_MINUS1_CHECK(nread, errno, -1)

    // if nread == 0 the connection is closed, bubble it
    if (nread == 0) {
        return 0;
    }

    switch (type) {
        case GNL_SOCKET_REQUEST_OPEN:
            *request = gnl_socket_request_init(GNL_SOCKET_REQUEST_OPEN, 0);
            GNL_NULL_CHECK(*request, ENOMEM, -1)

            gnl_message_sn_read(payload_message, (*request)->payload.open);
            break;

        case GNL_SOCKET_REQUEST_READ_N:
            GNL_REQUEST_N_READ_MESSAGE(payload_message, (*request)->payload.read_N, type);
            break;

        case GNL_SOCKET_REQUEST_READ:
            GNL_REQUEST_S_READ_MESSAGE(payload_message, (*request)->payload.read, type);
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            GNL_REQUEST_NNB_READ_MESSAGE(payload_message, (*request)->payload.write, type);
            break;

        case GNL_SOCKET_REQUEST_APPEND:
            GNL_REQUEST_SB_READ_MESSAGE(payload_message, (*request)->payload.append, type);
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            GNL_REQUEST_S_READ_MESSAGE(payload_message, (*request)->payload.lock, type);
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            GNL_REQUEST_S_READ_MESSAGE(payload_message, (*request)->payload.unlock, type);
            break;

        case GNL_SOCKET_REQUEST_CLOSE:
            GNL_REQUEST_N_READ_MESSAGE(payload_message, (*request)->payload.close, type);
            break;

        case GNL_SOCKET_REQUEST_REMOVE:
            GNL_REQUEST_S_READ_MESSAGE(payload_message, (*request)->payload.remove, type);
            break;

        default:
            errno = EINVAL;
            return -1;
            /* UNREACHED */
            break;
    }

    free(payload_message);

    return nread; //TODO: sistemare le read per ritornare i bytes letti
}

/**
 * {@inheritDoc}
 */
int gnl_socket_request_write(const struct gnl_socket_request *request, char **dest) {
    GNL_NULL_CHECK(request, EINVAL, -1)

    // the destination must be empty
    if (*dest != NULL) {
        errno = EINVAL;

        return -1;
    }

    char *built_message;
    int nwrite;

    switch (request->type) {
        case GNL_SOCKET_REQUEST_OPEN:
            nwrite = gnl_message_sn_write(request->payload.open, &built_message);
            break;

        case GNL_SOCKET_REQUEST_READ_N:
            nwrite = gnl_message_n_write(request->payload.read_N, &built_message);
            break;

        case GNL_SOCKET_REQUEST_READ:
            nwrite = gnl_message_s_write(request->payload.read, &built_message);
            break;

        case GNL_SOCKET_REQUEST_WRITE:
            nwrite = gnl_message_nnb_write(request->payload.write, &built_message);
            break;

        case GNL_SOCKET_REQUEST_APPEND:
            nwrite = gnl_message_sb_write(request->payload.append, &built_message);
            break;

        case GNL_SOCKET_REQUEST_LOCK:
            nwrite = gnl_message_s_write(request->payload.lock, &built_message);
            break;

        case GNL_SOCKET_REQUEST_UNLOCK:
            nwrite = gnl_message_s_write(request->payload.unlock, &built_message);
            break;

        case GNL_SOCKET_REQUEST_CLOSE:
            nwrite = gnl_message_n_write(request->payload.close, &built_message);
            break;

        case GNL_SOCKET_REQUEST_REMOVE:
            nwrite = gnl_message_s_write(request->payload.remove, &built_message);
            break;

        default:
            errno = EINVAL;
            return -1;
            /* UNREACHED */
            break;
    }

    if (nwrite <= 0) {
        return -1;
    }

    nwrite = encode(built_message, dest, nwrite, request->type);

    free(built_message);

    return nwrite;
}

/**
 * {@inheritDoc}
 */
int gnl_socket_request_send(const struct gnl_socket_request *request,
                            const struct gnl_socket_connection *connection,
                            int (*emit)(const struct gnl_socket_connection *connection, const char *message, size_t count)) {
    char *message = NULL;

    int nwrite = gnl_socket_request_write(request, &message);
    GNL_MINUS1_CHECK(nwrite, EINVAL, -1)

    int res = emit(connection, message, nwrite);

    free(message);

    return res;
}

#undef MAX_DIGITS_CHAR
#undef MAX_DIGITS_INT
#undef GNL_REQUEST_N_INIT
#undef GNL_REQUEST_S_INIT
#undef GNL_REQUEST_SB_INIT
#undef GNL_REQUEST_NNB_INIT
#undef GNL_REQUEST_N_READ_MESSAGE
#undef GNL_REQUEST_S_READ_MESSAGE
#undef GNL_REQUEST_SB_READ_MESSAGE
#undef GNL_REQUEST_NNB_READ_MESSAGE

#include <gnl_macro_end.h>