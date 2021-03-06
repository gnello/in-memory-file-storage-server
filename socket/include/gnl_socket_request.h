
#ifndef GNL_SOCKET_REQUEST_H
#define GNL_SOCKET_REQUEST_H

#include <sys/types.h>
#include <gnl_message_sn.h>
#include <gnl_message_s.h>
#include <gnl_message_n.h>
#include <gnl_message_snb.h>
#include <gnl_message_nnb.h>

/**
 * The possibles type of a socket request.
 */
enum gnl_socket_request_type {
    GNL_SOCKET_REQUEST_OPEN,
    GNL_SOCKET_REQUEST_READ,
    GNL_SOCKET_REQUEST_READ_N,
    GNL_SOCKET_REQUEST_WRITE,
    GNL_SOCKET_REQUEST_LOCK,
    GNL_SOCKET_REQUEST_UNLOCK,
    GNL_SOCKET_REQUEST_CLOSE,
    GNL_SOCKET_REQUEST_REMOVE
};

/**
 * The socket request.
 */
struct gnl_socket_request;

/**
 * Create a socket request struct with the given type and arguments.
 *
 * @param type  The type of the request.
 * @param num   The number of the subsequent params.
 * @param ...   The list of params supported by the given request type:
 *              - GNL_SOCKET_REQUEST_OPEN: char *pathname, int flags
 *              - GNL_SOCKET_REQUEST_READ: int fd
 *              - GNL_SOCKET_RESPONSE_READ_N: int number_of_files_to_read
 *              - GNL_SOCKET_RESPONSE_WRITE: int fd, size_t size, char *bytes
 *              - GNL_SOCKET_REQUEST_LOCK: int fd
 *              - GNL_SOCKET_REQUEST_UNLOCK: int fd
 *              - GNL_SOCKET_REQUEST_CLOSE: int fd
 *              - GNL_SOCKET_REQUEST_REMOVE: int fd
 *
 * @return      Returns a gnl_socket_request struct on success,
 *              NULL otherwise.
 */
struct gnl_socket_request *gnl_socket_request_init(enum gnl_socket_request_type type, int num, ...);

/**
 * Destroy the socket request.
 *
 * @param request   The socket_request to destroy.
 */
extern void gnl_socket_request_destroy(struct gnl_socket_request *request);

/**
 * Get the type of the given request. The output string dest will be
 * written with the string type of the given request.
 *
 * @param request   The request from where to get the type.
 * @param dest      The destination where to write the string type.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_request_get_type(const struct gnl_socket_request *request, char **dest);

/**
 * Get a string that represent the given request.
 *
 * @param request   The request to be represented.
 * @param dest      The pointer where to write the request string.
 *
 * @return          Returns the length of the request string on success,
 *                  -1 otherwise.
 */
extern size_t gnl_socket_request_to_string(const struct gnl_socket_request *request, char **dest);

/**
 * Build a request from the given string.
 *
 * @param message   The message from where to build the request.
 * @param type      The type of the request.
 *
 * @return          Returns the built gnl_socket_request on success,
 *                  NULL otherwise.
 */
extern struct gnl_socket_request *gnl_socket_request_from_string(const char *message, enum gnl_socket_request_type type);

/**
 * Get the type code of the given request.
 *
 * @param request   The socket request.
 *
 * @return          Returns the request type code on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_request_type(const struct gnl_socket_request *request);

/**
 * Read the file descriptor from the given GNL_SOCKET_REQUEST_READ, GNL_SOCKET_REQUEST_WRITE,
 * GNL_SOCKET_REQUEST_LOCK, GNL_SOCKET_REQUEST_UNLOCK, or GNL_SOCKET_REQUEST_CLOSE request.
 * If the request is not one of the above requests, this invocation will fail.
 *
 * @param request   The socket request.
 *
 * @return          The request file descriptor on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_request_get_fd(const struct gnl_socket_request *request);

/**
 * Read the filename from the given GNL_SOCKET_REQUEST_OPEN or GNL_SOCKET_REQUEST_REMOVE request.
 * If the request is not one of the above requests, this invocation will fail.
 *
 * @param request   The socket request.
 *
 * @return          The request filename on success,
 *                  -1 otherwise.
 */
extern char *gnl_socket_request_get_filename(const struct gnl_socket_request *request);

/**
 * Read the flags from the given GNL_SOCKET_REQUEST_OPEN request.
 * If the request is not a GNL_SOCKET_REQUEST_OPEN request,
 * this invocation will fail.
 *
 * @param request   The socket request.
 *
 * @return          The request flags on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_request_get_flags(const struct gnl_socket_request *request);

/**
 * Get the count of bytes from the given GNL_SOCKET_REQUEST_WRITE request.
 * If the request is not a GNL_SOCKET_REQUEST_WRITE request,
 * this invocation will fail.
 *
 * @param request   The request from where to get the count.
 *
 * @return          Returns the request count of bytes on success,
 *                  NULL otherwise.
 */
extern size_t gnl_socket_request_get_size(const struct gnl_socket_request *request);

/**
 * Get the bytes from the given GNL_SOCKET_REQUEST_WRITE request.
 * If the request is not a GNL_SOCKET_REQUEST_WRITE request,
 * this invocation will fail.
 *
 * @param request   The request from where to get the bytes.
 *
 * @return          Returns the request bytes on success,
 *                  NULL otherwise.
 */
extern void *gnl_socket_request_get_bytes(const struct gnl_socket_request *request);

#endif //GNL_SOCKET_REQUEST_H