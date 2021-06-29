
#ifndef GNL_SOCKET_REQUEST_H
#define GNL_SOCKET_REQUEST_H

#include "./gnl_socket_service.h"

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
        struct gnl_socket_message_sn *open;
        struct gnl_socket_message_s *read;
        struct gnl_socket_message_n *read_N;
        struct gnl_socket_message_sb *write;
        struct gnl_socket_message_sb *append;
        struct gnl_socket_message_s *lock;
        struct gnl_socket_message_s *unlock;
        struct gnl_socket_message_s *close;
        struct gnl_socket_message_s *remove;
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
 *              - GNL_SOCKET_RESPONSE_WRITE: char *pathname, char *bytes
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
 * Decode the given socket request.
 *
 * @param request   The socket request received from a socket message.
 *
 * @return          Returns a gnl_socket_request struct on success,
 *                  NULL otherwise.
 */
extern struct gnl_socket_request *gnl_socket_request_read(const char *request);

/**
 * Encode the given socket request.
 *
 * @param request   The socket request to encode.
 * @param dest      The destination where to write the encoded request,
 *                  his value must be initialized with NULL.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_request_write(const struct gnl_socket_request *request, char **dest);

/**
 * Send the given request to the given connection.
 *
 * @param request       The socket request to send to the server.
 * @param connection    The socket service connection instance.
 * @param emit          The function for emitting to the server, is typically a
 *                      function defined by a socket service.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_request_send(const struct gnl_socket_request *request,
        const struct gnl_socket_connection *connection,
        int (*emit)(const struct gnl_socket_connection *connection, const char *message));

#endif //GNL_SOCKET_REQUEST_H