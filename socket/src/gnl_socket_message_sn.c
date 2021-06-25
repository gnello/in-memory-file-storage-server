#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * The message_sn message.
 */
struct gnl_socket_message_sn {
    char *string;
    int number;
};

/**
 * Calculate the size of the message_sn.
 *
 * @param message_sn    The message_sn.
 *
 * @return              The size of the message_sn.
 */
static int gnl_socket_message_sn_size(const struct gnl_socket_message_sn message_sn) {
    return MAX_DIGITS_INT + strlen(message_sn.string) + MAX_DIGITS_INT;
}

/**
 * Create a new message_sn.
 */
struct gnl_socket_message_sn *gnl_socket_message_sn_init() {
    struct gnl_socket_message_sn *message_sn = (struct gnl_socket_message_sn *)calloc(1, sizeof(struct gnl_socket_message_sn));
    GNL_NULL_CHECK(message_sn, ENOMEM, NULL)

    return message_sn;
}

/**
 * Create a new message_sn with the given arguments.
 *
 * @param string    The string of the message_sn.
 * @param number    The number of the message_sn.
 */
struct gnl_socket_message_sn *gnl_socket_message_sn_init_with_args(char *string, int number) {
    struct gnl_socket_message_sn *message_sn = gnl_socket_message_sn_init();
    GNL_NULL_CHECK(message_sn, ENOMEM, NULL)

    message_sn->string = malloc((strlen(string) + 1) * sizeof(char));
    GNL_NULL_CHECK(message_sn->string, ENOMEM, NULL)

    strncpy(message_sn->string, string, strlen(string) + 1);
    message_sn->number = number;

    return message_sn;
}

/**
 * Destroy the given message_sn.
 *
 * @param message_sn    The message_sn to be destroyed.
 */
void gnl_socket_message_sn_destroy(struct gnl_socket_message_sn *message_sn) {
    if (message_sn != NULL) {
        free(message_sn->string);
        free(message_sn);
    }
}

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param message_sn    The message_sn.
 * @param dest          The destination where to write the socket message.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
int gnl_socket_message_sn_write(const struct gnl_socket_message_sn message_sn, char **dest) {
    int read_N_size = gnl_socket_message_sn_size(message_sn);

    GNL_CALLOC(*dest, read_N_size + 1, -1)

    int maxlen = read_N_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*lu%s%0*d", MAX_DIGITS_INT, strlen(message_sn.string), message_sn.string,
             MAX_DIGITS_INT, message_sn.number);

    return 0;
}

/**
 * Read the socket message and fill the message_sn with it.
 *
 * @param message       The message to read.
 * @param message_sn    The struct to fill with the message, it must be previously
 *                      initialized with gnl_socket_message_sn_init.
 *
 * @return              Returns a pointer to the created gnl_socket_message_sn
 *                      on success, NULL otherwise.
 */
int gnl_socket_message_sn_read(const char *message, struct gnl_socket_message_sn *message_sn) {
    if (message_sn == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the string length
    size_t string_len;
    sscanf(message, "%"MAX_DIGITS_CHAR"lu", &string_len);

    // get the string
    message_sn->string = calloc(string_len + 1, sizeof(char));
    GNL_NULL_CHECK(message_sn->string, ENOMEM, -1)

    strncpy(message_sn->string, message + MAX_DIGITS_INT, string_len);

    // get the number
    char message_N[MAX_DIGITS_INT];
    strncpy(message_N, message + MAX_DIGITS_INT + string_len, MAX_DIGITS_INT);

    char *ptr = NULL;
    message_sn->number = strtol(message_N, &ptr, 10);

    // if no digits found
    if ((char *)message_N == ptr) {
        errno = EINVAL;
        free(ptr);

        return -1;
    }

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>