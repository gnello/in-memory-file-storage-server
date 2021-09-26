
#ifndef GNL_SOCKET_RESPONSE_H
#define GNL_SOCKET_RESPONSE_H

#include "./gnl_socket_connection.h"
#include <gnl_message_nq.h>
#include <gnl_message_n.h>
#include <gnl_message_snb.h>

/**
 * The possibles type of a socket response.
 */
enum gnl_socket_response_type {

    // the request is successfully processed but
    // there are a file list sent by the server
    GNL_SOCKET_RESPONSE_OK_FILE_LIST,

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
struct gnl_socket_response;

/**
 * Create a socket response struct with the given type and arguments.
 *
 * @param type  The type of the response.
 * @param num   The number of the subsequent params.
 * @param ...   The list of params supported by the given response type:
 *              - GNL_SOCKET_RESPONSE_OK_FILE: char *filename, char *bytes
 *              - GNL_SOCKET_RESPONSE_OK_FD: int file_descriptor
 *              - GNL_SOCKET_RESPONSE_ERROR: int error_code
 *              The GNL_SOCKET_RESPONSE_OK_FILE_LIST response can not be initialized
 *              with args.
 *
 * @return      Returns a gnl_socket_response struct on success,
 *              NULL otherwise.
 */
extern struct gnl_socket_response *gnl_socket_response_init(enum gnl_socket_response_type type, int num, ...);

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
extern int gnl_socket_response_get_type(const struct gnl_socket_response *response, char **dest);

/**
 * Read the file descriptor from the given GNL_SOCKET_RESPONSE_OK_FD response.
 * If the response is not a GNL_SOCKET_RESPONSE_OK_FD response,
 * this invocation will fail.
 *
 * @param response  The socket response.
 *
 * @return          The response file descriptor on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_response_get_fd(const struct gnl_socket_response *response);

/**
 * Get the error code of the given GNL_SOCKET_RESPONSE_ERROR response.
 * If the response is not a GNL_SOCKET_RESPONSE_ERROR response,
 * this invocation will fail.
 *
 * @param response  The socket response,
 *
 * @return          Returns the response error code on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_response_get_error(const struct gnl_socket_response *response);

/**
 * Get the type code of the given response.
 *
 * @param response  The socket response,
 *
 * @return          Returns the response type code on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_response_type(const struct gnl_socket_response *response);

/**
 * Get a string that represent the given response.
 *
 * @param response  The response to be represented.
 * @param dest      The pointer where to write the response string.
 *
 * @return          Returns the length of the response string on success,
 *                  -1 otherwise.
 */
extern size_t gnl_socket_response_to_string(struct gnl_socket_response *response, char **dest);

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

/**
 * Add a file to the given GNL_SOCKET_RESPONSE_OK_FILE_LIST response.
 * If the response is not a GNL_SOCKET_RESPONSE_OK_FILE_LIST response,
 * this invocation will fail.
 *
 * @param response  The response where to add a file.
 * @param name      The name of the file.
 * @param count     The number of bytes of the file.
 * @param bytes     The content of the file.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_response_add_file(struct gnl_socket_response *response, const char *name, size_t count, const void *bytes);

/**
 * Get a file from the given GNL_SOCKET_RESPONSE_OK_FILE_LIST response.
 * If the response is not a GNL_SOCKET_RESPONSE_OK_FILE_LIST response,
 * this invocation will fail.
 *
 * @param response  The response from where to get a file.
 *
 * @return          Returns a gnl_message_snb containing the file
 *                  information on success, NULL otherwise.
 */
extern struct gnl_message_snb *gnl_socket_response_get_file(struct gnl_socket_response *response);

/**
 * Get the count of bytes from the given GNL_SOCKET_RESPONSE_OK_FILE response.
 * If the response is not a GNL_SOCKET_RESPONSE_OK_FILE response,
 * this invocation will fail.
 *
 * @param response  The response from where to get the count.
 *
 * @return          Returns the response count of bytes on success,
 *                  NULL otherwise.
 */
extern size_t gnl_socket_response_get_size(const struct gnl_socket_response *response);

/**
 * Get the bytes from the given GNL_SOCKET_RESPONSE_OK_FILE response.
 * If the response is not a GNL_SOCKET_RESPONSE_OK_FILE response,
 * this invocation will fail.
 *
 * @param response  The response from where to get the bytes.
 *
 * @return          Returns the response bytes on success,
 *                  NULL otherwise.
 */
extern void *gnl_socket_response_get_bytes(const struct gnl_socket_response *response);

#endif //GNL_SOCKET_RESPONSE_H