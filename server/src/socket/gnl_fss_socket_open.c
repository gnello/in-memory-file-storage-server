#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "./macro_beg.c"

#define MAX_DIGITS "10"
#define FLAG_LENGTH 1

/**
 * The open message.
 */
struct gnl_fss_socket_open {
    char *pathname;
    int flags;
};

/**
 * Calculate the size of the open message.
 *
 * @param open  The open message.
 *
 * @return      The size of the open message.
 */
static int message_size(const struct gnl_fss_socket_open open) {
    return
    sizeof(size_t) // the length of the pathname
    + strlen(open.pathname) // the pathname
    + sizeof(open.flags); // the flags
}

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param message   The open message.
 * @param dest      The destination where to write the socket message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_fss_socket_open_build_message(const struct gnl_fss_socket_open message, char **dest) {
    GNL_ALLOCATE_MESSAGE(*dest, message_size(message) + 1)

    int max_digits = atoi(MAX_DIGITS);
    if (max_digits == 0) {
        errno = EINVAL;
        return -1;
    }

    int maxlen = message_size(message) + 1 + max_digits + FLAG_LENGTH;

    snprintf(*dest, maxlen, "%0*lu%s%d", max_digits, strlen(message.pathname), message.pathname, message.flags);

    return 0;
}

/**
 * Create a new open struct.
 */
struct gnl_fss_socket_open *gnl_fss_socket_open_init() {
    struct gnl_fss_socket_open *open = (struct gnl_fss_socket_open *)malloc(sizeof(struct gnl_fss_socket_open));
    GNL_NULL_CHECK(open, ENOMEM, NULL)

    return open;
}

/**
 * Create a new open struct with the given arguments.
 *
 * @param pathname  The pathname.
 * @param flags     The flags.
 */
struct gnl_fss_socket_open *gnl_fss_socket_open_init_with_args(char *pathname, int flags) {
    struct gnl_fss_socket_open *open = gnl_fss_socket_open_init();
    GNL_NULL_CHECK(open, ENOMEM, NULL)

    open->pathname = malloc((strlen(pathname) + 1) * sizeof(char));
    GNL_NULL_CHECK(open->pathname, ENOMEM, NULL)

    strcpy(open->pathname, pathname);
    open->flags = flags;

    return open;
}

/**
 * Destroy the given message.
 *
 * @param message   The message to be destroyed.
 */
void gnl_fss_socket_open_destroy(struct gnl_fss_socket_open *message) {
    if (message != NULL) {
        free(message->pathname);
        free(message);
    }
}

/**
 * Read the socket message and fill the "open" struct with it.
 *
 * @param message   The message to read.
 * @param open      The struct to fill with the message.
 *
 * @return          Returns a pointer to the created gnl_fss_socket_open struct
 *                  on success, NULL otherwise.
 */
int gnl_fss_socket_open_read_message(const char *message, struct gnl_fss_socket_open *open) {
    // get the pathname length
    size_t pathname_len;
    sscanf(message, "%"MAX_DIGITS"lu", &pathname_len);

    // get the pathname string
    open->pathname = calloc(pathname_len + 1, sizeof(char));
    GNL_NULL_CHECK(open->pathname, ENOMEM, -1)

    int max_digits = atoi(MAX_DIGITS);
    if (max_digits == 0) {
        errno = EINVAL;
        return -1;
    }

    strncpy(open->pathname, message + max_digits, pathname_len);

    // get the flags
    char read_flags[FLAG_LENGTH];
    strncpy(read_flags, message + max_digits + pathname_len, FLAG_LENGTH);

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
#undef MAX_DIGITS

#include "./macro_end.c"