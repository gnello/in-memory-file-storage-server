
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
    GNL_SOCKET_REQUEST_APPEND,
    GNL_SOCKET_REQUEST_LOCK,
    GNL_SOCKET_REQUEST_UNLOCK,
    GNL_SOCKET_REQUEST_CLOSE,
    GNL_SOCKET_REQUEST_REMOVE
};

/**
 * The socket request.
 */
struct gnl_socket_request {
    enum gnl_socket_request_type type;
    union {
        struct gnl_message_sn *open;
        struct gnl_message_s *read;
        struct gnl_message_n *read_N;
        struct gnl_message_nnb *write;
        struct gnl_message_snb *append;
        struct gnl_message_s *lock;
        struct gnl_message_s *unlock;
        struct gnl_message_n *close;
        struct gnl_message_s *remove;
        //TODO: aggiungere request per fetchare i file espulsi dal server
    } payload;
};

/**
 * Create a socket request struct with the given type and arguments.
 *
 * @param type  The type of the request.
 * @param num   The number of the subsequent params.
 * @param ...   The list of params supported by the given request type:
 *              - GNL_SOCKET_REQUEST_OPEN: char *pathname, int flags
 *              - GNL_SOCKET_REQUEST_READ: char *pathname
 *              - GNL_SOCKET_RESPONSE_READ_N: int number_of_files_to_read
 *              - GNL_SOCKET_RESPONSE_WRITE: int fd, size_t size, char *bytes
 *              - GNL_SOCKET_RESPONSE_APPEND: char *pathname, char *bytes
 *              - GNL_SOCKET_REQUEST_LOCK: char *pathname
 *              - GNL_SOCKET_REQUEST_UNLOCK: char *pathname
 *              - GNL_SOCKET_REQUEST_CLOSE: char *pathname
 *              - GNL_SOCKET_REQUEST_REMOVE: char *pathname
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
 * @param response  The request from where to get the type.
 * @param dest      The destination where to write the string type.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_request_get_type(struct gnl_socket_request *request, char **dest);

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

#endif //GNL_SOCKET_REQUEST_H