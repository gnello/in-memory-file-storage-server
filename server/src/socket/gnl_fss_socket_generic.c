#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "./macro_beg.c"

#define MAX_DIGITS "10"
#define FLAG_LENGTH 1

/**
 * The generic message.
 */
struct gnl_fss_socket_generic {
    // the pathname of the file
    char *pathname;
};

/**
 * Calculate the size of the generic message.
 *
 * @param generic   The generic message.
 *
 * @return          Returns he size of the generic message on success,
 *                  -1 otherwise.
 */
static int gnl_fss_socket_generic_message_size(const struct gnl_fss_socket_generic generic) {
    int max_digits;
    CONV_TO_INT(MAX_DIGITS, max_digits)

    return max_digits + strlen(generic.pathname);
}

/**
 * Create a new generic struct.
 */
struct gnl_fss_socket_generic *gnl_fss_socket_generic_init() {
    struct gnl_fss_socket_generic *generic = (struct gnl_fss_socket_generic *)calloc(1, sizeof(struct gnl_fss_socket_generic));
    GNL_NULL_CHECK(generic, ENOMEM, NULL)

    return generic;
}

/**
 * Create a new generic struct with the given arguments.
 *
 * @param pathname  The pathname.
 */
struct gnl_fss_socket_generic *gnl_fss_socket_generic_init_with_args(char *pathname) {
    struct gnl_fss_socket_generic *generic = gnl_fss_socket_generic_init();
    GNL_NULL_CHECK(generic, ENOMEM, NULL)

    generic->pathname = malloc((strlen(pathname) + 1) * sizeof(char));
    GNL_NULL_CHECK(generic->pathname, ENOMEM, NULL)

    strncpy(generic->pathname, pathname, strlen(pathname) + 1);

    return generic;
}

/**
 * Destroy the given message.
 *
 * @param message   The message to be destroyed.
 */
void gnl_fss_socket_generic_destroy(struct gnl_fss_socket_generic *message) {
    if (message != NULL) {
        free(message->pathname);
        free(message);
    }
}

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param message   The generic message.
 * @param dest      The destination where to write the socket message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_fss_socket_generic_build_message(const struct gnl_fss_socket_generic message, char **dest) {
    int message_size = gnl_fss_socket_generic_message_size(message);
    GNL_MINUS1_CHECK(message_size, EINVAL, -1)

    GNL_ALLOCATE_MESSAGE(*dest, message_size + 1)

    int max_digits;
    CONV_TO_INT(MAX_DIGITS, max_digits)

    int maxlen = message_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*lu%s", max_digits, strlen(message.pathname), message.pathname);

    return 0;
}

/**
 * Read the socket message and fill the "generic" struct with it.
 *
 * @param message   The message to generic.
 * @param generic   The struct to fill with the message, it must be previously
 *                  initialized with gnl_fss_socket_generic_init.
 *
 * @return          Returns a pointer to the created gnl_fss_socket_generic struct
 *                  on success, NULL otherwise.
 */
int gnl_fss_socket_generic_read_message(const char *message, struct gnl_fss_socket_generic *generic) {
    if (generic == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the pathname length
    size_t pathname_len;
    sscanf(message, "%"MAX_DIGITS"lu", &pathname_len);

    // get the pathname string
    generic->pathname = calloc(pathname_len + 1, sizeof(char));
    GNL_NULL_CHECK(generic->pathname, ENOMEM, -1)

    int max_digits;
    CONV_TO_INT(MAX_DIGITS, max_digits)

    strncpy(generic->pathname, message + max_digits, pathname_len);

    return 0;
}

#undef FLAG_LENGTH
#undef MAX_DIGITS

#include "./macro_end.c"