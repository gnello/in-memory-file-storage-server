#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gnl_macro_beg.h>

#define MAX_DIGITS_CHAR "10"
#define MAX_DIGITS_INT 10

/**
 * The request_s.
 */
struct gnl_socket_request_s {
    char *string;
};

/**
 * Calculate the size of the request_s.
 *
 * @param request_s The request_s.
 *
 * @return          The size of the request_s.
 */
static int gnl_socket_request_s_size(const struct gnl_socket_request_s request_s) {
    return MAX_DIGITS_INT + strlen(request_s.string);
}

/**
 * Create a new request_s.
 */
struct gnl_socket_request_s *gnl_socket_request_s_init() {
    struct gnl_socket_request_s *request_s = (struct gnl_socket_request_s *)calloc(1, sizeof(struct gnl_socket_request_s));
    GNL_NULL_CHECK(request_s, ENOMEM, NULL)

    return request_s;
}

/**
 * Create a new request_s with the given arguments.
 *
 * @param string    The string of the request_s.
 */
struct gnl_socket_request_s *gnl_socket_request_s_init_with_args(char *string) {
    struct gnl_socket_request_s *request_s = gnl_socket_request_s_init();
    GNL_NULL_CHECK(request_s, ENOMEM, NULL)

    request_s->string = malloc((strlen(string) + 1) * sizeof(char));
    GNL_NULL_CHECK(request_s->string, ENOMEM, NULL)

    strncpy(request_s->string, string, strlen(string) + 1);

    return request_s;
}

/**
 * Destroy the given request_s.
 *
 * @param request   The request_s to be destroyed.
 */
void gnl_socket_request_s_destroy(struct gnl_socket_request_s *request) {
    if (request != NULL) {
        free(request->string);
        free(request);
    }
}

/**
 * Prepare the socket request and put it into "dest".
 *
 * @param request   The request_s.
 * @param dest      The destination where to write the socket request.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_socket_request_s_write(const struct gnl_socket_request_s request, char **dest) {
    int request_size = gnl_socket_request_s_size(request);

    GNL_ALLOCATE_MESSAGE(*dest, request_size + 1)

    int maxlen = request_size + 1; // count also the '\0' char

    snprintf(*dest, maxlen, "%0*lu%s", MAX_DIGITS_INT, strlen(request.string), request.string);

    return 0;
}

/**
 * Read the socket request and fill the request_s with it.
 *
 * @param request   The request_s message to read.
 * @param request_s The struct to fill with the request, it must be previously
 *                  initialized with gnl_socket_request_s_init.
 *
 * @return          Returns a pointer to the created gnl_socket_request_s
 *                  on success, NULL otherwise.
 */
int gnl_socket_request_s_read(const char *request, struct gnl_socket_request_s *request_s) {
    if (request_s == NULL) {
        errno = EINVAL;

        return -1;
    }

    // get the string length
    size_t pathname_len;
    sscanf(request, "%"MAX_DIGITS_CHAR"lu", &pathname_len);

    // get the string string
    request_s->string = calloc(pathname_len + 1, sizeof(char));
    GNL_NULL_CHECK(request_s->string, ENOMEM, -1)

    strncpy(request_s->string, request + MAX_DIGITS_INT, pathname_len);

    return 0;
}

#undef MAX_DIGITS_INT
#undef MAX_DIGITS_CHAR

#include <gnl_macro_end.h>