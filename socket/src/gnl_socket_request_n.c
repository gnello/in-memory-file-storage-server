#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * The request_n.
 */
struct gnl_socket_request_n {
    int number;
};

/**
 * Calculate the size of the request.
 *
 * @param request_n The request_n.
 *
 * @return          The size of the request_n.
 */
static int gnl_socket_request_n_size(const struct gnl_socket_request_n request_n) {
    return MAX_DIGITS_INT;
}

/**
 * Create a new request_n.
 */
struct gnl_socket_request_n *gnl_socket_request_n_init() {
    struct gnl_socket_request_n *request_n = (struct gnl_socket_request_n *)calloc(1, sizeof(struct gnl_socket_request_n));
    GNL_NULL_CHECK(request_n, ENOMEM, NULL)

    return request_n;
}

/**
 * Create a new request_n with the given arguments.
 *
 * @param number    The number of random files to read.
 */
struct gnl_socket_request_n *gnl_socket_request_n_init_with_args(int number) {
    struct gnl_socket_request_n *request_n = gnl_socket_request_n_init();
    GNL_NULL_CHECK(request_n, ENOMEM, NULL)

    request_n->number = number;

    return request_n;
}

/**
 * Destroy the given request_n.
 *
 * @param request_n    The request_n to be destroyed.
 */
void gnl_socket_request_n_destroy(struct gnl_socket_request_n *request_n) {
    if (request_n != NULL) {
        free(request_n);
    }
}

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param request_n The request_n.
 * @param dest      The destination where to write the socket message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_socket_request_n_write(const struct gnl_socket_request_n request_n, char **dest) {
    int read_N_size = gnl_socket_request_n_size(request_n);

    GNL_ALLOCATE_MESSAGE(*dest, read_N_size + 1)

    int maxlen = read_N_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*d", MAX_DIGITS_INT, request_n.number);

    return 0;
}

/**
 * Read the socket message and fill the request_n with it.
 *
 * @param message   The message to read.
 * @param request_n The struct to fill with the message, it must be previously
 *                  initialized with gnl_socket_request_n_init.
 *
 * @return          Returns a pointer to the created gnl_socket_request_n
 *                  on success, NULL otherwise.
 */
int gnl_socket_request_n_read(const char *message, struct gnl_socket_request_n *request_n) {
    if (request_n == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the number
    char message_N[MAX_DIGITS_INT];
    strncpy(message_N, message, MAX_DIGITS_INT);

    char *ptr = NULL;
    request_n->number = strtol(message_N, &ptr, 10);

    // if no digits found
    if ((char *)request_n == ptr) {
        errno = EINVAL;
        free(ptr);

        return -1;
    }

    return 0;
}

#undef N_LENGTH
#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>