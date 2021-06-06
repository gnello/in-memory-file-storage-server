#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "./macro_beg.c"

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
int gnl_fss_socket_open_prepare_message(const struct gnl_fss_socket_open message, char **dest) {
    GNL_ALLOCATE_MESSAGE(*dest, message_size(message) + 1)

    sprintf(*dest, "%lu%s%d", strlen(message.pathname), message.pathname, message.flags);

    return 0;
}

/**
 * Read the socket message.
 *
 * @param message   The message.
 *
 * @return          Returns a pointer to the created gnl_fss_socket_open struct
 *                  on success, NULL otherwise.
 */
struct gnl_fss_socket_open *gnl_fss_socket_open_read_message(const char *message) {
    char *buffer = malloc((strlen(message) + 1) * sizeof(char));
    GNL_NULL_CHECK(buffer, ENOMEM, NULL)

    strcpy(buffer, message);

    struct gnl_fss_socket_open *open = (struct gnl_fss_socket_open *)malloc(sizeof(struct gnl_fss_socket_open));
    GNL_NULL_CHECK(open, ENOMEM, NULL)

    size_t pathname_len;
    sscanf(message, "%lu", &pathname_len);

    open->pathname = malloc((pathname_len + 1) * sizeof(char));
    GNL_NULL_CHECK(open->pathname, ENOMEM, NULL)

    sscanf(buffer, "%lu%s%d", &pathname_len, open->pathname, &open->flags);

    return open;
}

#include "./macro_end.c"