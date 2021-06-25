#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * The request_sb message.
 */
struct gnl_socket_request_sb {
    char *string;
    char *bytes;
};

/**
 * Calculate the size of the request_sb.
 *
 * @param request_sb    The request_sb.
 *
 * @return              The size of the request_sb.
 */
static int gnl_socket_request_sb_size(const struct gnl_socket_request_sb request_sb) {
    return MAX_DIGITS_INT + strlen(request_sb.string) + MAX_DIGITS_INT + strlen(request_sb.bytes);
}

/**
 * Create a new request_sb.
 */
struct gnl_socket_request_sb *gnl_socket_request_sb_init() {
    struct gnl_socket_request_sb *request_sb = (struct gnl_socket_request_sb *)calloc(1, sizeof(struct gnl_socket_request_sb));
    GNL_NULL_CHECK(request_sb, ENOMEM, NULL)

    return request_sb;
}

/**
 * Create a new request_sb with the given arguments.
 *
 * @param string    The string of the request_sb.
 * @param bytes     The bytes of the request_sb.
 */
struct gnl_socket_request_sb *gnl_socket_request_sb_init_with_args(char *string, char *bytes) {
    struct gnl_socket_request_sb *request_sb = gnl_socket_request_sb_init();
    GNL_NULL_CHECK(request_sb, ENOMEM, NULL)

    // assign string
    request_sb->string = malloc((strlen(string) + 1) * sizeof(char));
    GNL_NULL_CHECK(request_sb->string, ENOMEM, NULL)

    strncpy(request_sb->string, string, strlen(string) + 1);

    // assign bytes
    request_sb->bytes = calloc(strlen(bytes) + 1, sizeof(char));
    GNL_NULL_CHECK(request_sb->bytes, ENOMEM, NULL)

    memcpy(request_sb->bytes, bytes, strlen(bytes) + 1);

    return request_sb;
}

/**
 * Destroy the given request_sb.
 *
 * @param request_sb    The request_sb to be destroyed.
 */
void gnl_socket_request_sb_destroy(struct gnl_socket_request_sb *request_sb) {
    if (request_sb != NULL) {
        free(request_sb->string);
        free(request_sb->bytes);
        free(request_sb);
    }
}

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param request_sb    The request_sb.
 * @param dest          The destination where to write the socket message.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
int gnl_socket_request_sb_write(const struct gnl_socket_request_sb request_sb, char **dest) {
    int request_sb_size = gnl_socket_request_sb_size(request_sb);

    GNL_CALLOC(*dest, request_sb_size + 1, -1)

    int maxlen = request_sb_size - strlen(request_sb.bytes) + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*lu%s%0*lu", MAX_DIGITS_INT, strlen(request_sb.string), request_sb.string,
             MAX_DIGITS_INT, strlen(request_sb.bytes));

    memcpy(*dest + MAX_DIGITS_INT + strlen(request_sb.string) + MAX_DIGITS_INT, request_sb.bytes, strlen(request_sb.bytes) + 1);

    return 0;
}

/**
 * Read the socket message and fill the request_sb with it.
 *
 * @param message       The message to read.
 * @param request_sb    The struct to fill with the message, it must be previously
 *                      initialized with gnl_socket_request_sb_init.
 *
 * @return              Returns a pointer to the created gnl_socket_request_sb
 *                      on success, NULL otherwise.
 */
int gnl_socket_request_sb_read(const char *message, struct gnl_socket_request_sb *request_sb) {
    if (request_sb == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the string length
    size_t string_len;
    sscanf(message, "%"MAX_DIGITS_CHAR"lu", &string_len);

    // get the string
    request_sb->string = calloc(string_len + 1, sizeof(char));
    GNL_NULL_CHECK(request_sb->string, ENOMEM, -1)

    strncpy(request_sb->string, message + MAX_DIGITS_INT, string_len);

    // get the bytes length
    char bytes_len_string[MAX_DIGITS_INT];
    strncpy(bytes_len_string, message + MAX_DIGITS_INT + string_len, MAX_DIGITS_INT);

    int bytes_len;
    char *ptr = NULL;
    bytes_len = strtol(bytes_len_string, &ptr, 10);

    // if no digits found
    if ((char *)bytes_len_string == ptr) {
        errno = EINVAL;
        free(ptr);

        return -1;
    }

    // get the bytes
    request_sb->bytes = calloc(bytes_len + 1, sizeof(char));
    GNL_NULL_CHECK(request_sb->bytes, ENOMEM, -1)

    memcpy(request_sb->bytes, message + MAX_DIGITS_INT + string_len + MAX_DIGITS_INT, bytes_len + 1);

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>