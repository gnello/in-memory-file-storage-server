//TODO: gnl_socket_message_sl (string list) --> len_string + string + num of following elements + len_bytes + bytes + ...
//TODO: e se invece nella risposta invio il numero di vittime così il client le puà chiedere? (message_sn)
//TODO: gestire bene la max size, se un file supera ad es. 2000MB dividere le richieste
//TODO: nel caso non le voglia allora si attacca

#include <stdarg.h>
#include "./gnl_socket_message_n.c"
#include "../include/gnl_socket_response.h"
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

#define GNL_RESPONSE_N_INIT(num, ref, a_list) {                                                         \
    switch (num) {                                                                                      \
        case 0:                                                                                         \
            ref = gnl_socket_message_n_init();                               \
            break;                                                                                      \
        case 1:                                                                                         \
            ref = gnl_socket_message_n_init_with_args(va_arg(a_list, int));   \
        break;                                                                                          \
            default:                                                                                    \
            errno = EINVAL;                                                                             \
            return NULL;                                                                                \
    }                                                                                                   \
                                                                                                        \
    GNL_NULL_CHECK(ref, ENOMEM, NULL)                                       \
}

/**
 * Calculate the size of the response.
 *
 * @param message   The message of the response.
 *
 * @return          Returns he size of the response on success,
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
static int encode(const char *message, char **dest, enum gnl_socket_response_type type) {
    int response_size = MAX_DIGITS_INT + MAX_DIGITS_INT;
    unsigned long message_size = 0;

    if (message != NULL) {
        response_size = size(message);
        message_size = strlen(message);
    }

    GNL_CALLOC(*dest, response_size + 1, -1)

    int maxlen = response_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*d%0*lu%s", MAX_DIGITS_INT, type, MAX_DIGITS_INT, message_size, message);

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
static int decode(const char *message, char **dest, enum gnl_socket_response_type *type) {
    size_t message_len;

    // get the operation type and the message length
    sscanf(message, "%"MAX_DIGITS_CHAR"d%"MAX_DIGITS_CHAR"lu", (int *)type, &message_len);

    // allocate memory
    GNL_CALLOC(*dest, message_len + 1, -1)

    // get the message
    strncpy(*dest, message + MAX_DIGITS_INT + MAX_DIGITS_INT, message_len);

    return 0;
}

int gnl_socket_response_to_string(struct gnl_socket_response *response, char **dest) {
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
            break;
    }

    return 0;
}

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
            break;
    }

    // clean memory reserved for valist
    va_end(a_list);

    return socket_response;
}

void gnl_socket_response_destroy(struct gnl_socket_response *response) {
    switch (response->type) {
        case GNL_SOCKET_RESPONSE_OK_EVICTED:
            gnl_socket_message_n_destroy(response->payload.ok_evicted);
            break;
        case GNL_SOCKET_RESPONSE_OK:
            break;
        case GNL_SOCKET_RESPONSE_ERROR:
            gnl_socket_message_n_destroy(response->payload.error);
            break;
    }

    free(response);
}

struct gnl_socket_response *gnl_socket_response_read(const char *message) {
    struct gnl_socket_response *socket_response;

    char *payload_message;
    enum gnl_socket_response_type type;

    decode(message, &payload_message, &type);

    switch (type) {
        case GNL_SOCKET_RESPONSE_OK_EVICTED:
            socket_response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK_EVICTED, 0);
            GNL_NULL_CHECK(socket_response, ENOMEM, NULL)

            gnl_socket_message_n_read(payload_message, socket_response->payload.ok_evicted);
            break;

        case GNL_SOCKET_RESPONSE_OK:
            socket_response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_OK, 0);
            GNL_NULL_CHECK(socket_response, ENOMEM, NULL)
            break;

        case GNL_SOCKET_RESPONSE_ERROR:
            socket_response = gnl_socket_response_init(GNL_SOCKET_RESPONSE_ERROR, 0);
            GNL_NULL_CHECK(socket_response, ENOMEM, NULL)

            gnl_socket_message_n_read(payload_message, socket_response->payload.error);
            break;

        default:
            errno = EINVAL;
            return NULL;
            /* UNREACHED */
            break;
    }

    free(payload_message);

    return socket_response;
}

int gnl_socket_response_write(struct gnl_socket_response *response, char **dest) {
    GNL_NULL_CHECK(response, EINVAL, -1)

    // the destination must be empty
    if (*dest != NULL) {
        errno = EINVAL;

        return -1;
    }

    char *built_message = NULL;
    int res;

    switch (response->type) {
        case GNL_SOCKET_RESPONSE_OK_EVICTED:
            res = gnl_socket_message_n_write(*(response->payload.ok_evicted), &built_message);
            break;
        case GNL_SOCKET_RESPONSE_OK:
            res = 0;
            break;

        case GNL_SOCKET_RESPONSE_ERROR:
            res = gnl_socket_message_n_write(*(response->payload.error), &built_message);
            break;

        default:
            errno = EINVAL;
            return -1;
            /* UNREACHED */
            break;
    }

    if (res != 0) {
        return -1;
    }

    encode(built_message, dest, response->type);

    free(built_message);

    return 0;
}

//TODO: scrivere test
int gnl_socket_response_evicted(struct gnl_socket_response *response) {
    GNL_NULL_CHECK(response, EINVAL, -1)

    if (response->type == GNL_SOCKET_RESPONSE_OK_EVICTED) {
        return response->payload.ok_evicted->number;
    }

    errno = EINVAL;

    return -1;
}

//TODO: aggiungere test dopo aver scritto la enum
int gnl_socket_response_error(struct gnl_socket_response *response) {
    GNL_NULL_CHECK(response, EINVAL, -1)

    if (response->type == GNL_SOCKET_RESPONSE_ERROR) {
        return response->payload.error->number;
    }

    errno = EINVAL;

    return -1;
}

//TODO: scrivere test
struct gnl_socket_response *gnl_socket_response_get(const struct gnl_socket_service_connection *connection,
                                                    int (*on_message)(const struct gnl_socket_service_connection *connection,
                                                                      char **message, int size)) {
    char *message = NULL;
    GNL_CALLOC(message, 10, NULL)

    int res = on_message(connection, &message, 10);
    GNL_MINUS1_CHECK(res, errno, NULL)

    struct gnl_socket_response *response = gnl_socket_response_read(message);
    GNL_NULL_CHECK(response, errno, NULL);

    return response;
}

#undef MAX_DIGITS_CHAR
#undef MAX_DIGITS_INT
#include <gnl_macro_end.h>