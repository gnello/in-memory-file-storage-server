
#ifndef GNL_SOCKET_RESPONSE_H
#define GNL_SOCKET_RESPONSE_H

#include "./gnl_socket_service.h"

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
    //TODO: aggiungere response per i file fetched
};

/**
 * The socket response.
 */
struct gnl_socket_response {
    enum gnl_socket_response_type type;
    union {
        struct gnl_socket_message_n *open;
        struct gnl_socket_message_b *read; //TODO: b o sb?
//        struct gnl_socket_message_n *read_N; //TODO: n è il numero di file pronti per essere letti, ci vogliono n chiamate per prenderli tutti
//        struct gnl_socket_message_n *write; //TODO: n è il numero di file evicted
//        struct gnl_socket_message_n *append; //TODO: n è il numero di file evicted
//        struct gnl_socket_message_s *lock; //TODO: risposta vuota, non implementare
//        struct gnl_socket_message_s *unlock; //TODO: risposta vuota, non implementare
//        struct gnl_socket_message_s *close; //TODO: risposta vuota, non implementare
//        struct gnl_socket_message_s *remove; //TODO: risposta vuota, non implementare
        struct gnl_socket_message_n *error;
    } payload;
};

/**
 * Create a socket response struct with the given type and arguments.
 *
 * @param type  The type of the response.
 * @param num   The number of the subsequent params.
 * @param ...   The list of params supported by the given response type:
 *              - GNL_SOCKET_RESPONSE_OPEN: int number_of_files_evicted
 *              - GNL_SOCKET_RESPONSE_ERROR: int error_code
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
 * @param response  The response to format to string.
 * @param dest      The destination where to write the string response.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_response_to_string(struct gnl_socket_response *response, char **dest);

/**
 * Decode the given socket response.
 *
 * @param response  The socket response received from a socket message.
 *
 * @return          Returns a gnl_socket_response struct on success,
 *                  NULL otherwise.
 */
extern struct gnl_socket_response *gnl_socket_response_read(const char *response);

/**
 * Encode the given socket response.
 *
 * @param response  The socket_response to encode.
 * @param dest      The destination where to write the encoded response,
 *                  his value must be initialized with NULL.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_response_write(struct gnl_socket_response *response, char **dest);

/**
 * Read the number of evicted files from the given response.
 *
 * @param response  The socket response.
 *
 * @return          The number of evicted files on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_response_evicted(struct gnl_socket_response *response);

/**
 * Get the error code of the given response.
 *
 * @param response  The socket response,
 *
 * @return          Returns the response error code on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_response_error(struct gnl_socket_response *response);

/**
 * Get the response from the given connection.
 *
 * @param connection    The socket service connection instance.
 * @param on_message    The function for read from the server, is typically a
 *                      function defined by a socket service.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern struct gnl_socket_response *gnl_socket_response_get(const struct gnl_socket_service_connection *connection,
                                   int (*on_message)(const struct gnl_socket_service_connection *connection,
                                           char **message, int size));

#endif //GNL_SOCKET_RESPONSE_H