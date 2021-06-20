
#ifndef GNL_SOCKET_SERVICE_H
#define GNL_SOCKET_SERVICE_H

/**
 * Connect to the given socket name.
 *
 * @param socket_name   The name of the socket where to connect.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_service_connect(const char *socket_name);

/**
 * Close a connection to the given socket name.
 *
 * @param socket_name   The name of the socket where to disconnect.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_service_close(const char *socket_name);

/**
 * Send a message through the previously opened connection.
 *
 * @param message   The message to send.
 *
 * @return          Returns the number of bytes sent on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_service_emit(const char *message);

/**
 * Read a message from the previously opened connection.
 *
 * @param message   The pointer where to write the read message.
 * @param size      The number of bytes to read.
 *
 * @return          Returns the number of bytes read on success,
 *                  -1 otherwise.
 */
extern int gnl_socket_service_read(char **message, int size);

#endif //GNL_SOCKET_SERVICE_H