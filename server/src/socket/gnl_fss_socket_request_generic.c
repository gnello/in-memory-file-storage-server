#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../macro_beg.c"

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * The generic request.
 */
struct gnl_fss_socket_request_generic {
    // the pathname of the file
    char *pathname;
};

/**
 * Calculate the size of the generic request.
 *
 * @param generic   The generic request.
 *
 * @return          Returns he size of the generic request on success,
 *                  -1 otherwise.
 */
static int gnl_fss_socket_request_generic_size(const struct gnl_fss_socket_request_generic generic) {
    return MAX_DIGITS_INT + strlen(generic.pathname);
}

/**
 * Create a new request_generic struct.
 */
struct gnl_fss_socket_request_generic *gnl_fss_socket_request_generic_init() {
    struct gnl_fss_socket_request_generic *generic = (struct gnl_fss_socket_request_generic *)calloc(1, sizeof(struct gnl_fss_socket_request_generic));
    GNL_NULL_CHECK(generic, ENOMEM, NULL)

    return generic;
}

/**
 * Create a new request_generic struct with the given arguments.
 *
 * @param pathname  The pathname.
 */
struct gnl_fss_socket_request_generic *gnl_fss_socket_request_generic_init_with_args(char *pathname) {
    struct gnl_fss_socket_request_generic *generic = gnl_fss_socket_request_generic_init();
    GNL_NULL_CHECK(generic, ENOMEM, NULL)

    generic->pathname = malloc((strlen(pathname) + 1) * sizeof(char));
    GNL_NULL_CHECK(generic->pathname, ENOMEM, NULL)

    strncpy(generic->pathname, pathname, strlen(pathname) + 1);

    return generic;
}

/**
 * Destroy the given request.
 *
 * @param request   The request to be destroyed.
 */
void gnl_fss_socket_request_generic_destroy(struct gnl_fss_socket_request_generic *request) {
    if (request != NULL) {
        free(request->pathname);
        free(request);
    }
}

/**
 * Prepare the socket request and put it into "dest".
 *
 * @param request   The generic request.
 * @param dest      The destination where to write the socket request.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_fss_socket_request_generic_write(const struct gnl_fss_socket_request_generic request, char **dest) {
    int request_size = gnl_fss_socket_request_generic_size(request);

    GNL_ALLOCATE_MESSAGE(*dest, request_size + 1)

    int maxlen = request_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*lu%s", MAX_DIGITS_INT, strlen(request.pathname), request.pathname);

    return 0;
}

/**
 * Read the socket request and fill the "generic" struct with it.
 *
 * @param request   The generic request message to read.
 * @param generic   The struct to fill with the request, it must be previously
 *                  initialized with gnl_fss_socket_request_generic_init.
 *
 * @return          Returns a pointer to the created gnl_fss_socket_request_generic struct
 *                  on success, NULL otherwise.
 */
int gnl_fss_socket_request_generic_read(const char *request, struct gnl_fss_socket_request_generic *generic) {
    if (generic == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the pathname length
    size_t pathname_len;
    sscanf(request, "%"MAX_DIGITS_CHAR"lu", &pathname_len);

    // get the pathname string
    generic->pathname = calloc(pathname_len + 1, sizeof(char));
    GNL_NULL_CHECK(generic->pathname, ENOMEM, -1)

    strncpy(generic->pathname, request + MAX_DIGITS_INT, pathname_len);

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include "../macro_end.c"