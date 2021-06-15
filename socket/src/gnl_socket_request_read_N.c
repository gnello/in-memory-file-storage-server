#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * The read_N message.
 */
struct gnl_socket_request_read_N {
    // the number of random files to read
    int N;

    // the dirname where to put the read files
    char *dirname;
};

/**
 * Calculate the size of the read N message.
 *
 * @param read_N    The read_N message.
 *
 * @return          The size of the read_N message.
 */
static int gnl_socket_request_read_N_size(const struct gnl_socket_request_read_N read_N) {
    return MAX_DIGITS_INT + strlen(read_N.dirname) + MAX_DIGITS_INT;
}

/**
 * Create a new read_N struct.
 */
struct gnl_socket_request_read_N *gnl_socket_request_read_N_init() {
    struct gnl_socket_request_read_N *read_N = (struct gnl_socket_request_read_N *)calloc(1, sizeof(struct gnl_socket_request_read_N));
    GNL_NULL_CHECK(read_N, ENOMEM, NULL)

    return read_N;
}

/**
 * Create a new read_N struct with the given arguments.
 *
 * @param dirname   The dirname where to put the read files.
 * @param N         The number of random files to read.
 */
struct gnl_socket_request_read_N *gnl_socket_request_read_N_init_with_args(char *dirname, int N) {
    struct gnl_socket_request_read_N *read_N = gnl_socket_request_read_N_init();
    GNL_NULL_CHECK(read_N, ENOMEM, NULL)

    read_N->dirname = malloc((strlen(dirname) + 1) * sizeof(char));
    GNL_NULL_CHECK(read_N->dirname, ENOMEM, NULL)

    strncpy(read_N->dirname, dirname, strlen(dirname) + 1);
    read_N->N = N;

    return read_N;
}

/**
 * Destroy the given message.
 *
 * @param read_N    The message to be destroyed.
 */
void gnl_socket_request_read_N_destroy(struct gnl_socket_request_read_N *read_N) {
    if (read_N != NULL) {
        free(read_N->dirname);
        free(read_N);
    }
}

/**
 * Prepare the socket message and put it into "dest".
 *
 * @param read_N    The read_N message.
 * @param dest      The destination where to write the socket message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_socket_request_read_N_write(const struct gnl_socket_request_read_N read_N, char **dest) {
    int read_N_size = gnl_socket_request_read_N_size(read_N);

    GNL_ALLOCATE_MESSAGE(*dest, read_N_size + 1)

    int maxlen = read_N_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*lu%s%0*d", MAX_DIGITS_INT, strlen(read_N.dirname), read_N.dirname,
             MAX_DIGITS_INT, read_N.N);

    return 0;
}

/**
 * Read the socket message and fill the "read_N" struct with it.
 *
 * @param message   The message to read.
 * @param read_N    The struct to fill with the message, it must be previously
 *                  initialized with gnl_socket_request_read_N_init.
 *
 * @return          Returns a pointer to the created gnl_socket_request_read_N struct
 *                  on success, NULL otherwise.
 */
int gnl_socket_request_read_N_read(const char *message, struct gnl_socket_request_read_N *read_N) {
    if (read_N == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the dirname length
    size_t dirname_len;
    sscanf(message, "%"MAX_DIGITS_CHAR"lu", &dirname_len);

    // get the dirname string
    read_N->dirname = calloc(dirname_len + 1, sizeof(char));
    GNL_NULL_CHECK(read_N->dirname, ENOMEM, -1)

    strncpy(read_N->dirname, message + MAX_DIGITS_INT, dirname_len);

    // get the N
    char message_N[MAX_DIGITS_INT];
    strncpy(message_N, message + MAX_DIGITS_INT + dirname_len, MAX_DIGITS_INT);

    char *ptr = NULL;
    read_N->N = strtol(message_N, &ptr, 10);

    // if no digits found
    if ((char *)read_N == ptr) {
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