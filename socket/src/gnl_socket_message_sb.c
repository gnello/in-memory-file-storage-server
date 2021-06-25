#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * The message_sb message.
 */
struct gnl_socket_message_sb {
    char *string;
    char *bytes;
};

/**
 * Calculate the size of the message_sb.
 *
 * @param message_sb    The message_sb.
 *
 * @return              The size of the message_sb.
 */
static int gnl_socket_message_sb_size(const struct gnl_socket_message_sb message_sb) {
    return MAX_DIGITS_INT + strlen(message_sb.string) + MAX_DIGITS_INT + strlen(message_sb.bytes);
}

/**
 * Create a new message_sb.
 */
struct gnl_socket_message_sb *gnl_socket_message_sb_init() {
    struct gnl_socket_message_sb *message_sb = (struct gnl_socket_message_sb *)calloc(1, sizeof(struct gnl_socket_message_sb));
    GNL_NULL_CHECK(message_sb, ENOMEM, NULL)

    return message_sb;
}

/**
 * Create a new message_sb with the given arguments.
 *
 * @param string    The string of the message_sb.
 * @param bytes     The bytes of the message_sb.
 */
struct gnl_socket_message_sb *gnl_socket_message_sb_init_with_args(char *string, char *bytes) {
    struct gnl_socket_message_sb *message_sb = gnl_socket_message_sb_init();
    GNL_NULL_CHECK(message_sb, ENOMEM, NULL)

    // assign string
    message_sb->string = malloc((strlen(string) + 1) * sizeof(char));
    GNL_NULL_CHECK(message_sb->string, ENOMEM, NULL)

    strncpy(message_sb->string, string, strlen(string) + 1);

    // assign bytes
    message_sb->bytes = calloc(strlen(bytes) + 1, sizeof(char));
    GNL_NULL_CHECK(message_sb->bytes, ENOMEM, NULL)

    memcpy(message_sb->bytes, bytes, strlen(bytes) + 1);

    return message_sb;
}

/**
 * Destroy the given message_sb.
 *
 * @param message_sb    The message_sb to be destroyed.
 */
void gnl_socket_message_sb_destroy(struct gnl_socket_message_sb *message_sb) {
    if (message_sb != NULL) {
        free(message_sb->string);
        free(message_sb->bytes);
        free(message_sb);
    }
}

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param message_sb    The message_sb.
 * @param dest          The destination where to write the socket message.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
int gnl_socket_message_sb_write(const struct gnl_socket_message_sb message_sb, char **dest) {
    int message_sb_size = gnl_socket_message_sb_size(message_sb);

    GNL_CALLOC(*dest, message_sb_size + 1, -1)

    int maxlen = message_sb_size - strlen(message_sb.bytes) + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*lu%s%0*lu", MAX_DIGITS_INT, strlen(message_sb.string), message_sb.string,
             MAX_DIGITS_INT, strlen(message_sb.bytes));

    memcpy(*dest + MAX_DIGITS_INT + strlen(message_sb.string) + MAX_DIGITS_INT, message_sb.bytes, strlen(message_sb.bytes) + 1);

    return 0;
}

/**
 * Read the socket message and fill the message_sb with it.
 *
 * @param message       The message to read.
 * @param message_sb    The struct to fill with the message, it must be previously
 *                      initialized with gnl_socket_message_sb_init.
 *
 * @return              Returns a pointer to the created gnl_socket_message_sb
 *                      on success, NULL otherwise.
 */
int gnl_socket_message_sb_read(const char *message, struct gnl_socket_message_sb *message_sb) {
    if (message_sb == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the string length
    size_t string_len;
    sscanf(message, "%"MAX_DIGITS_CHAR"lu", &string_len);

    // get the string
    message_sb->string = calloc(string_len + 1, sizeof(char));
    GNL_NULL_CHECK(message_sb->string, ENOMEM, -1)

    strncpy(message_sb->string, message + MAX_DIGITS_INT, string_len);

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
    message_sb->bytes = calloc(bytes_len + 1, sizeof(char));
    GNL_NULL_CHECK(message_sb->bytes, ENOMEM, -1)

    memcpy(message_sb->bytes, message + MAX_DIGITS_INT + string_len + MAX_DIGITS_INT, bytes_len + 1);

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>