#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <gnl_message_n.h>
#include <gnl_message_sb.h>
#include "../include/gnl_socket_response.h"
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

#define GNL_RESPONSE_N_INIT(num, ref, a_list) {                                                         \
    switch (num) {                                                                                      \
        case 0:                                                                                         \
            ref = gnl_message_n_init();                               \
            break;                                                                                      \
        case 1:                                                                                         \
            ref = gnl_message_n_init_with_args(va_arg(a_list, int));   \
        break;                                                                                          \
            default:                                                                                    \
            errno = EINVAL;                                                                             \
            return NULL;                                                                                \
    }                                                                                                   \
                                                                                                        \
    GNL_NULL_CHECK(ref, ENOMEM, NULL)                                       \
}

/**
 * {@inheritDoc}
 */
int gnl_socket_response_get_type(struct gnl_socket_response *response, char **dest) {
    if (response == NULL) {
        errno = EINVAL;

        return -1;
    }

    switch (response->type) {
        case GNL_SOCKET_RESPONSE_OK_EVICTED:
            GNL_CALLOC(*dest, 11, -1);
            strcpy(*dest, "OK_EVICTED");
            break;

        case GNL_SOCKET_RESPONSE_OK_FILE:
            GNL_CALLOC(*dest, 8, -1);
            strcpy(*dest, "OK_FILE");
            break;

        case GNL_SOCKET_RESPONSE_OK_FD:
            GNL_CALLOC(*dest, 6, -1);
            strcpy(*dest, "OK_FD");
            break;

        case GNL_SOCKET_RESPONSE_OK:
            GNL_CALLOC(*dest, 3, -1);
            strcpy(*dest, "OK");
            break;

        case GNL_SOCKET_RESPONSE_ERROR:
            GNL_CALLOC(*dest, 6, -1);
            strcpy(*dest, "ERROR");
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
struct gnl_socket_response *gnl_socket_response_init(enum gnl_socket_response_type type, int num, ...) {
    struct gnl_socket_response *socket_response = (struct gnl_socket_response *)malloc(sizeof(struct gnl_socket_response));
    GNL_NULL_CHECK(socket_response, ENOMEM, NULL)

    // initialize valist for num number of arguments
    va_list a_list;
    va_start(a_list, num);

    // assign operation type
    socket_response->type = type;

    // declare utils vars
    char *buffer_s;
    char *buffer_b;

    // assign payload object
    switch (type) {
        case GNL_SOCKET_RESPONSE_OK_EVICTED:
            GNL_RESPONSE_N_INIT(num, socket_response->payload.ok_evicted, a_list)
            break;

        case GNL_SOCKET_RESPONSE_OK_FILE:
            switch (num) {
                case 0:
                    socket_response->payload.ok_file = gnl_message_sb_init();
                    break;
                case 2:
                    buffer_s = va_arg(a_list, char *);
                    buffer_b = va_arg(a_list, void *);
                    socket_response->payload.ok_file = gnl_message_sb_init_with_args(buffer_s, buffer_b);
                    break;
                default:
                    errno = EINVAL;
                    return NULL;
            }

            GNL_NULL_CHECK(socket_response->payload.ok_file, ENOMEM, NULL)
            break;

        case GNL_SOCKET_RESPONSE_OK_FD:
            GNL_RESPONSE_N_INIT(num, socket_response->payload.ok_fd, a_list)
            break;

        case GNL_SOCKET_RESPONSE_OK:
            if (num != 0) {
                errno = EINVAL;
                free(socket_response);

                return NULL;
            }
            break;

        case GNL_SOCKET_RESPONSE_ERROR:
            GNL_RESPONSE_N_INIT(num, socket_response->payload.error, a_list)
            break;

        default:
            errno = EINVAL;
            free(socket_response);

            return NULL;
            /* UNREACHED */
    }

    // clean memory reserved for valist
    va_end(a_list);

    return socket_response;
}

/**
 * {@inheritDoc}
 */
void gnl_socket_response_destroy(struct gnl_socket_response *response) {
    switch (response->type) {
        case GNL_SOCKET_RESPONSE_OK_EVICTED:
            gnl_message_n_destroy(response->payload.ok_evicted);
            break;
        case GNL_SOCKET_RESPONSE_OK_FILE:
            gnl_message_sb_destroy(response->payload.ok_file);
            break;
        case GNL_SOCKET_RESPONSE_OK_FD:
            gnl_message_n_destroy(response->payload.ok_fd);
            break;
        case GNL_SOCKET_RESPONSE_OK:
            break;
        case GNL_SOCKET_RESPONSE_ERROR:
            gnl_message_n_destroy(response->payload.error);
            break;
    }

    free(response);
}

/**
 * {@inheritDoc}
 */
//TODO: scrivere test
int gnl_socket_response_get_evicted(struct gnl_socket_response *response) {
    GNL_NULL_CHECK(response, EINVAL, -1)

    if (response->type == GNL_SOCKET_RESPONSE_OK_EVICTED) {
        return response->payload.ok_evicted->number;
    }

    errno = EINVAL;

    return -1;
}

/**
 * {@inheritDoc}
 */
//TODO: scrivere test
int gnl_socket_response_get_fd(struct gnl_socket_response *response) {
    GNL_NULL_CHECK(response, EINVAL, -1)

    if (response->type == GNL_SOCKET_RESPONSE_OK_FD) {
        return response->payload.ok_fd->number;
    }

    errno = EINVAL;

    return -1;
}

/**
 * {@inheritDoc}
 */
//TODO: aggiungere test dopo aver scritto la enum
int gnl_socket_response_get_error(struct gnl_socket_response *response) {
    GNL_NULL_CHECK(response, EINVAL, -1)

    if (response->type == GNL_SOCKET_RESPONSE_ERROR) {
        return response->payload.error->number;
    }

    errno = EINVAL;

    return -1;
}

/**
 * {@inheritDoc}
 */
struct gnl_socket_response *gnl_socket_response_from_string(const char *message, enum gnl_socket_response_type type) {
    //validate the parameters
    GNL_NULL_CHECK(message, EINVAL, NULL)

    struct gnl_socket_response *response;
    int res;

    switch (type) {
        case GNL_SOCKET_RESPONSE_OK_EVICTED:
            response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_EVICTED, 0);
            GNL_NULL_CHECK(response, ENOMEM, NULL)

            res = gnl_message_n_from_string(message, response->payload.ok_evicted);
            GNL_MINUS1_CHECK(res, errno, NULL)
            break;

        case GNL_SOCKET_RESPONSE_OK_FILE:
            response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_FILE, 0);
            GNL_NULL_CHECK(response, ENOMEM, NULL)

            res = gnl_message_sb_from_string(message, response->payload.ok_file);
            GNL_MINUS1_CHECK(res, errno, NULL)
            break;

        case GNL_SOCKET_RESPONSE_OK_FD:
            response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_FD, 0);
            GNL_NULL_CHECK(response, ENOMEM, NULL)

            res = gnl_message_n_from_string(message, response->payload.ok_fd);
            GNL_MINUS1_CHECK(res, errno, NULL)
            break;

        case GNL_SOCKET_RESPONSE_OK:
            response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);
            GNL_NULL_CHECK(response, ENOMEM, NULL)
            break;

        case GNL_SOCKET_RESPONSE_ERROR:
            response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_ERROR, 0);
            GNL_NULL_CHECK(response, ENOMEM, NULL)

            res = gnl_message_n_from_string(message, response->payload.error);
            GNL_MINUS1_CHECK(res, errno, NULL)
            break;

        default:
            errno = EINVAL;
            return NULL;
            /* UNREACHED */
    }

    return response;
}

/**
 * {@inheritDoc}
 */
size_t gnl_socket_response_to_string(const struct gnl_socket_response *response, char **dest) {
    // validate the parameters
    GNL_NULL_CHECK(response, EINVAL, -1)
    GNL_MINUS1_CHECK(-1 * (*dest != NULL), EINVAL, -1)
    
    size_t len;

    switch (response->type) {
        case GNL_SOCKET_RESPONSE_OK_EVICTED:
            len = gnl_message_n_to_string(response->payload.ok_evicted, dest);
            break;

        case GNL_SOCKET_RESPONSE_OK_FILE:
            len = gnl_message_sb_to_string(response->payload.ok_file, dest);
            break;

        case GNL_SOCKET_RESPONSE_OK_FD:
            len = gnl_message_n_to_string(response->payload.ok_fd, dest);
            break;

        case GNL_SOCKET_RESPONSE_OK:
            len = 0;
            break;

        case GNL_SOCKET_RESPONSE_ERROR:
            len = gnl_message_n_to_string(response->payload.error, dest);
            break;

        default:
            errno = EINVAL;
            return -1;
            /* UNREACHED */
    }

    return len;
}

#undef MAX_DIGITS_CHAR
#undef MAX_DIGITS_INT
#undef GNL_RESPONSE_N_INIT
#include <gnl_macro_end.h>