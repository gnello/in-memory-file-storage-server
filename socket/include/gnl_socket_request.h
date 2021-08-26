
#ifndef GNL_SOCKET_REQUEST_H
#define GNL_SOCKET_REQUEST_H

#include <sys/types.h>
#include <gnl_message_sn.h>
#include <gnl_message_s.h>
#include <gnl_message_n.h>
#include <gnl_message_sb.h>
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
        struct gnl_message_sb *append;
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
 * Format to string the given request. The output string dest will be
 * written with the string type of the given request.
 *
 * @param request   The request to format to string.
 * @param dest      The destination where to write the string request.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_request_to_string(struct gnl_socket_request *request, char **dest);

/**
 * Read the given socket request into the given gnl_socket_request struct.
 * This function should be used to get a request from a socket.
 *
 * @param fd        The file descriptor where to read.
 * @param request   The request to instantiate from reading. It must be
 *                  initialized to NULL;
 * @param readn     The function to use to read from the given file descriptor.
 *
 * @return          Returns the number of bytes read on success,
 *                  -1 otherwise.
 */
extern size_t gnl_socket_request_read(int fd, struct gnl_socket_request **request,
        ssize_t (*readn)(int fd, void *ptr, size_t n));

/**
 * Write the given request into the given file descriptor.
 * This function should be used to send a request through a socket.
 *
 * @param fd        The file descriptor where to write.
 * @param request   The socket request to encode.
 * @param writen    The function to use to write to the given file descriptor.
 *
 * @return          Returns the number of bytes wrote on success,
 *                  -1 otherwise.
 */
extern size_t gnl_socket_request_write(int fd, const struct gnl_socket_request *request,
        ssize_t (*writen)(int fd, void *ptr, size_t n));

#endif //GNL_SOCKET_REQUEST_H