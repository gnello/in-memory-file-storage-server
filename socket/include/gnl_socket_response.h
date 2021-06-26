
#ifndef GNL_SOCKET_RESPONSE_H
#define GNL_SOCKET_RESPONSE_H

/**
 * The possibles type of a socket response.
 */
enum gnl_socket_response_type {
    GNL_SOCKET_RESPONSE_OPEN,
    GNL_SOCKET_RESPONSE_READ,
    GNL_SOCKET_RESPONSE_READ_N,
    GNL_SOCKET_RESPONSE_WRITE,
    GNL_SOCKET_RESPONSE_APPEND,
    GNL_SOCKET_RESPONSE_LOCK,
    GNL_SOCKET_RESPONSE_UNLOCK,
    GNL_SOCKET_RESPONSE_CLOSE,
    GNL_SOCKET_RESPONSE_REMOVE,
    GNL_SOCKET_RESPONSE_ERROR
};

/**
 * The socket response.
 */
struct gnl_socket_response {
    enum gnl_socket_response_type type;
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
        struct gnl_socket_message_s *error;
    } payload;
};

/**
 * Create a socket response struct with the given type and arguments.
 *
 * @param type  The type of the response.
 * @param num   The number of the subsequent params.
 * @param ...   The list of params supported by the given response type:
 *              - GNL_SOCKET_RESPONSE_OPEN: char *pathname, int flags
 *              - GNL_SOCKET_RESPONSE_READ: char *pathname
 *              - GNL_SOCKET_RESPONSE_READ_N: TODO
 *              - GNL_SOCKET_RESPONSE_WRITE: TODO
 *              - GNL_SOCKET_RESPONSE_APPEND: TODO
 *              - GNL_SOCKET_RESPONSE_LOCK: char *pathname
 *              - GNL_SOCKET_RESPONSE_UNLOCK: char *pathname
 *              - GNL_SOCKET_RESPONSE_CLOSE: char *pathname
 *              - GNL_SOCKET_RESPONSE_REMOVE: char *pathname
 *
 * @return      Returns a gnl_socket_response struct on success,
 *              NULL otherwise.
 */
struct gnl_socket_response *gnl_socket_response_init(enum gnl_socket_response_type type, int num, ...);

/**
 * Destroy the socket response.
 *
 * @param response   The socket_response to destroy.
 */
extern void gnl_socket_response_destroy(struct gnl_socket_response *response);

/**
 * Format to string the given response. The output string dest will be
 * written with the string type of the given response.
 *
 * @param response   The response to format to string.
 * @param dest      The destination where to write the string response.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_response_to_string(struct gnl_socket_response *response, char **dest);

/**
 * Decode the given socket response.
 *
 * @param response   The socket response received from a socket message.
 *
 * @return          Returns a gnl_socket_response struct on success,
 *                  NULL otherwise.
 */
extern struct gnl_socket_response *gnl_socket_response_read(const char *response);

/**
 * Encode the given socket response.
 *
 * @param response   The socket_response to encode.
 * @param dest      The destination where to write the encoded response,
 *                  his value must be initialized with NULL.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_response_write(struct gnl_socket_response *response, char **dest);

#endif //GNL_SOCKET_RESPONSE_H