#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10
#define FLAG_LENGTH 1

/**
 * The open message.
 */
struct gnl_socket_request_open {
    // the pathname of the file to open
    char *pathname;

    // the open mode flags
    int flags;
};

/**
 * Calculate the size of the open message.
 *
 * @param open  The open message.
 *
 * @return      The size of the open message.
 */
static int gnl_socket_request_open_message_size(const struct gnl_socket_request_open open) {
    return MAX_DIGITS_INT + strlen(open.pathname) + FLAG_LENGTH;
}

/**
 * Create a new open struct.
 */
struct gnl_socket_request_open *gnl_socket_request_open_init() {
    struct gnl_socket_request_open *open = (struct gnl_socket_request_open *)calloc(1, sizeof(struct gnl_socket_request_open));
    GNL_NULL_CHECK(open, ENOMEM, NULL)

    return open;
}

/**
 * Create a new open struct with the given arguments.
 *
 * @param pathname  The pathname.
 * @param flags     The flags.
 */
struct gnl_socket_request_open *gnl_socket_request_open_init_with_args(char *pathname, int flags) {
    struct gnl_socket_request_open *open = gnl_socket_request_open_init();
    GNL_NULL_CHECK(open, ENOMEM, NULL)

    open->pathname = malloc((strlen(pathname) + 1) * sizeof(char));
    GNL_NULL_CHECK(open->pathname, ENOMEM, NULL)

    strncpy(open->pathname, pathname, strlen(pathname) + 1);
    open->flags = flags;

    return open;
}

/**
 * Destroy the given message.
 *
 * @param message   The message to be destroyed.
 */
void gnl_socket_request_open_destroy(struct gnl_socket_request_open *message) {
    if (message != NULL) {
        free(message->pathname);
        free(message);
    }
}

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param message   The open message.
 * @param dest      The destination where to write the socket message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_socket_request_open_write(const struct gnl_socket_request_open message, char **dest) {
    int message_size = gnl_socket_request_open_message_size(message);

    GNL_ALLOCATE_MESSAGE(*dest, message_size + 1)

    int maxlen = message_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*lu%s%d", MAX_DIGITS_INT, strlen(message.pathname), message.pathname, message.flags);

    return 0;
}

/**
 * Read the socket message and fill the "open" struct with it.
 *
 * @param message   The message to read.
 * @param open      The struct to fill with the message, it must be previously
 *                  initialized with gnl_socket_request_open_init.
 *
 * @return          Returns a pointer to the created gnl_socket_request_open struct
 *                  on success, NULL otherwise.
 */
int gnl_socket_request_open_read(const char *message, struct gnl_socket_request_open *open) {
    if (open == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the pathname length
    size_t pathname_len;
    sscanf(message, "%"MAX_DIGITS_CHAR"lu", &pathname_len);

    // get the pathname string
    open->pathname = calloc(pathname_len + 1, sizeof(char));
    GNL_NULL_CHECK(open->pathname, ENOMEM, -1)

    strncpy(open->pathname, message + MAX_DIGITS_INT, pathname_len);

    // get the flags
    char read_flags[FLAG_LENGTH];
    strncpy(read_flags, message + MAX_DIGITS_INT + pathname_len, FLAG_LENGTH);

    char *ptr = NULL;
    open->flags = strtol(read_flags, &ptr, 10);

    // if no digits found
    if ((char *)read_flags == ptr) {
        errno = EINVAL;

        return -1;
    }

    return 0;
}

#undef FLAG_LENGTH
#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>