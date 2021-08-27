
#ifndef GNL_SOCKET_RESPONSE_H
#define GNL_SOCKET_RESPONSE_H

#include "./gnl_socket_connection.h"

/**
 * The possibles type of a socket response.
 */
enum gnl_socket_response_type {

    // the request is successfully processed but
    // there are evicted files
    GNL_SOCKET_RESPONSE_OK_EVICTED,

    // the request is successfully processed and
    // a file is returned
    GNL_SOCKET_RESPONSE_OK_FILE,

    // the request is successfully processed and
    // a file descriptor is returned
    GNL_SOCKET_RESPONSE_OK_FD,

    // the request is successfully processed
    GNL_SOCKET_RESPONSE_OK,

    // there was an error during the processing
    // of the request
    GNL_SOCKET_RESPONSE_ERROR
};

/**
 * The socket response.
 */
struct gnl_socket_response {
    enum gnl_socket_response_type type;
    union {
        struct gnl_message_n *ok_evicted;
        struct gnl_message_snb *ok_file;
        struct gnl_message_n *ok_fd;
        struct gnl_message_n *error;
    } payload;
};

/**
 * Create a socket response struct with the given type and arguments.
 *
 * @param type  The type of the response.
 * @param num   The number of the subsequent params.
 * @param ...   The list of params supported by the given response type:
 *              - GNL_SOCKET_RESPONSE_OK_EVICTED: int number_of_files_evicted
 *              - GNL_SOCKET_RESPONSE_OK_FILE: char *filename, char *bytes
 *              - GNL_SOCKET_RESPONSE_OK_FD: int file_descriptor
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
 * Get the type of the given response. The output string dest will be
 * written with the string type of the given response.
 *
 * @param response  The response from where to get the type.
 * @param dest      The destination where to write the string type.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_response_get_type(struct gnl_socket_response *response, char **dest);

/**
 * Read the number of evicted files from the given response.
 *
 * @param response  The socket response.
 *
 * @return          The number of evicted files on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_response_get_evicted(struct gnl_socket_response *response);

/**
 * Read the file descriptor from the given response.
 *
 * @param response  The socket response.
 *
 * @return          The response file descriptor on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_response_get_fd(struct gnl_socket_response *response);

/**
 * Get the error code of the given response.
 *
 * @param response  The socket response,
 *
 * @return          Returns the response error code on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_response_get_error(struct gnl_socket_response *response);

/**
 * Get a string that represent the given response.
 *
 * @param response  The response to be represented.
 * @param dest      The pointer where to write the response string.
 *
 * @return          Returns the length of the response string on success,
 *                  -1 otherwise.
 */
extern size_t gnl_socket_response_to_string(const struct gnl_socket_response *response, char **dest);

/**
 * Build a response from the given string.
 *
 * @param message   The message from where to build the response.
 * @param type      The type of the response.
 *
 * @return          Returns the built gnl_socket_response on success,
 *                  NULL otherwise.
 */
extern struct gnl_socket_response *gnl_socket_response_from_string(const char *message, enum gnl_socket_response_type type);

#endif //GNL_SOCKET_RESPONSE_H