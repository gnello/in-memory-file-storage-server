#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * The message_n.
 */
struct gnl_socket_message_n {
    int number;
};

/**
 * Calculate the size of the message_n.
 *
 * @param message_n The message_n.
 *
 * @return          The size of the message_n.
 */
static int gnl_socket_message_n_size(const struct gnl_socket_message_n message_n) {
    return MAX_DIGITS_INT;
}

/**
 * Create a new message_n.
 */
struct gnl_socket_message_n *gnl_socket_message_n_init() {
    struct gnl_socket_message_n *message_n = (struct gnl_socket_message_n *)calloc(1, sizeof(struct gnl_socket_message_n));
    GNL_NULL_CHECK(message_n, ENOMEM, NULL)

    return message_n;
}

/**
 * Create a new message_n with the given arguments.
 *
 * @param number    The number of the message_n.
 */
struct gnl_socket_message_n *gnl_socket_message_n_init_with_args(int number) {
    struct gnl_socket_message_n *message_n = gnl_socket_message_n_init();
    GNL_NULL_CHECK(message_n, ENOMEM, NULL)

    message_n->number = number;

    return message_n;
}

/**
 * Destroy the given message_n.
 *
 * @param message_n    The message_n to be destroyed.
 */
void gnl_socket_message_n_destroy(struct gnl_socket_message_n *message_n) {
    if (message_n != NULL) {
        free(message_n);
    }
}

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param message_n The message_n.
 * @param dest      The destination where to write the socket message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_socket_message_n_write(const struct gnl_socket_message_n message_n, char **dest) {
    int read_N_size = gnl_socket_message_n_size(message_n);

    GNL_CALLOC(*dest, read_N_size + 1, -1)

    int maxlen = read_N_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*d", MAX_DIGITS_INT, message_n.number);

    return 0;
}

/**
 * Read the socket message and fill the message_n with it.
 *
 * @param message   The message to read.
 * @param message_n The struct to fill with the message, it must be previously
 *                  initialized with gnl_socket_message_n_init.
 *
 * @return          Returns a pointer to the created gnl_socket_message_n
 *                  on success, NULL otherwise.
 */
int gnl_socket_message_n_read(const char *message, struct gnl_socket_message_n *message_n) {
    if (message_n == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the number
    char message_N[MAX_DIGITS_INT];
    strncpy(message_N, message, MAX_DIGITS_INT);

    char *ptr = NULL;
    message_n->number = strtol(message_N, &ptr, 10);

    // if no digits found
    if ((char *)message_n == ptr) {
        errno = EINVAL;
        free(ptr);

        return -1;
    }

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>