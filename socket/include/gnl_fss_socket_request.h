
#ifndef GNL_FSS_SOCKET_REQUEST_H
#define GNL_FSS_SOCKET_REQUEST_H

/**
 * The possibles type of a socket request.
 */
enum gnl_fss_socket_request_type {
    GNL_FSS_SOCKET_REQUEST_OPEN,
    GNL_FSS_SOCKET_REQUEST_READ,
    GNL_FSS_SOCKET_REQUEST_READ_N,
    GNL_FSS_SOCKET_REQUEST_WRITE,
    GNL_FSS_SOCKET_REQUEST_APPEND,
    GNL_FSS_SOCKET_REQUEST_LOCK,
    GNL_FSS_SOCKET_REQUEST_UNLOCK,
    GNL_FSS_SOCKET_REQUEST_CLOSE,
    GNL_FSS_SOCKET_REQUEST_REMOVE
};

/**
 * The socket request.
 */
typedef struct gnl_fss_socket_request gnl_fss_socket_request;

/**
 * Create a socket request struct with the given type and arguments.
 *
 * @param type  The type of the request.
 * @param num   The number of the subsequent params.
 * @param ...   The list of params supported by the given request type:
 *              - GNL_FSS_SOCKET_REQUEST_OPEN: char *pathname, int flags
 *              - GNL_FSS_SOCKET_REQUEST_READ: char *pathname
 *              - GNL_FSS_SOCKET_REQUEST_READ_N: TODO
 *              - GNL_FSS_SOCKET_REQUEST_WRITE: TODO
 *              - GNL_FSS_SOCKET_REQUEST_APPEND: TODO
 *              - GNL_FSS_SOCKET_REQUEST_LOCK: char *pathname
 *              - GNL_FSS_SOCKET_REQUEST_UNLOCK: char *pathname
 *              - GNL_FSS_SOCKET_REQUEST_CLOSE: char *pathname
 *              - GNL_FSS_SOCKET_REQUEST_REMOVE: char *pathname
 *
 * @return      Returns a gnl_fss_socket_request struct on success,
 *              NULL otherwise.
 */
gnl_fss_socket_request *gnl_fss_socket_request_init(enum gnl_fss_socket_request_type type, int num, ...);

/**
 * Destroy the socket request.
 *
 * @param request   The socket_request to destroy.
 */
void gnl_fss_socket_request_destroy(gnl_fss_socket_request *request);

/**
 * Decode the given socket request.
 *
 * @param request   The socket request received from a socket message.
 *
 * @return          Returns a gnl_fss_socket_request struct on success,
 *                  NULL otherwise.
 */
gnl_fss_socket_request *gnl_fss_socket_request_read(const char *request);

/**
 * Encode the given socket request.
 *
 * @param request   The socket_request to encode.
 * @param dest      The destination where to write the encoded request.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_fss_socket_request_write(gnl_fss_socket_request *request, char **dest);

#endif //GNL_FSS_SOCKET_REQUEST_H