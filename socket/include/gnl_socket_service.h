
#ifndef GNL_SOCKET_SERVICE_H
#define GNL_SOCKET_SERVICE_H

#include <sys/types.h>
#include "./gnl_socket_connection.h"
#include "./gnl_socket_request.h"
#include "./gnl_socket_response.h"

/**
 * Check whether a connection is active or not.
 *
 * @param connection    The socket_service_connection instance.
 *
 * @return              Returns 1 if connection is active,
 *                      0 otherwise.
 */
extern int gnl_socket_service_is_active(const struct gnl_socket_connection *connection);

/**
 * Connect to the given socket name.
 *
 * @param socket_name   The name of the socket where to connect.
 *
 * @return              Returns the socket_service_connection instance
 *                      on success, NULL otherwise.
 */
extern struct gnl_socket_connection *gnl_socket_service_connect(const char *socket_name);

/**
 * Close a connection to the given connection. Attention! This operation
 * will destroy the connection.
 *
 * @param connection    The socket_service_connection instance.
 * @param socket_name   The name of the socket where to disconnect.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_service_close(struct gnl_socket_connection *connection);

/**
 * Write n bytes to a file descriptor.
 *
 * Credits: “Advanced Programming In the UNIX Environment”
 * by W. Richard Stevens and Stephen A. Rago, 2013, 3rd Edition,
 * Addison-Wesley
 *
 * @param fd    The file descriptor where to write.
 * @param ptr   The pointer containing the elements to write.
 * @param n     The number of bytes to write.
 *
 * @return      Returns the number of bytes wrote on success,
 *              -1 otherwise.
 */
extern ssize_t gnl_socket_service_writen(int fd, void *ptr, size_t n);

/**
 * Read n bytes from a file descriptor.
 *
 * Credits: “Advanced Programming In the UNIX Environment”
 * by W. Richard Stevens and Stephen A. Rago, 2013, 3rd Edition,
 * Addison-Wesley
 *
 * @param fd    The file descriptor where to read.
 * @param ptr   The pointer where to write the elements read.
 * @param n     The number of bytes to read.
 *
 * @return      Returns the number of bytes read on success,
 *              -1 otherwise.
 */
extern ssize_t gnl_socket_service_readn(int fd, void *ptr, size_t n);

/**
 * Send the given request through the given connection.
 *
 * @param connection    The socket_service_connection instance.
 * @param request       The request to send.
 *
 * @return              Returns a gnl_socket_response on success,
 *                      NULL otherwise.
 */
extern struct gnl_socket_response * gnl_socket_service_send(const struct gnl_socket_connection *connection,
        const struct gnl_socket_request *request);

#endif //GNL_SOCKET_SERVICE_H