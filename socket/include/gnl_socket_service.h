
#ifndef GNL_SOCKET_SERVICE_H
#define GNL_SOCKET_SERVICE_H

#include "./gnl_socket_connection.h"

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
 * Send a message through the given connection.
 *
 * @param connection    The socket_service_connection instance.
 * @param message       The message to send.
 * @param count         The number of bytes to write.
 *
 * @return              Returns the number of bytes sent on success,
 *                      -1 otherwise.
 */
extern int gnl_socket_service_emit(const struct gnl_socket_connection *connection, const char *message, size_t count);

/**
 * Read a message from the given connection.
 *
 * @param connection    The socket_service_connection instance.
 * @param message       The pointer where to write the read message.
 * @param size          The number of bytes to read.
 *
 * @return              Returns the number of bytes read on success,
 *                      -1 otherwise.
 */
extern int gnl_socket_service_read(const struct gnl_socket_connection *connection, char **message, int size);

#endif //GNL_SOCKET_SERVICE_H