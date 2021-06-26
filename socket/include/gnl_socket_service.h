
#ifndef GNL_SOCKET_SERVICE_H
#define GNL_SOCKET_SERVICE_H

/**
 * Socket connection instance.
 */
struct gnl_socket_service_connection {
    int fd; // the identifier of the socket connection
    char* socket_name; // the socket name
    int active; // if 1 the connection is active, otherwise (active == 0) is not.
};

/**
 * Check whether a connection is active or not.
 *
 * @param connection    The socket_service_connection instance.
 *
 * @return              Returns 1 if connection is active,
 *                      0 otherwise.
 */
extern int gnl_socket_service_is_active(const struct gnl_socket_service_connection *connection);

/**
 * Connect to the given socket name.
 *
 * @param socket_name   The name of the socket where to connect.
 *
 * @return              Returns the socket_service_connection instance
 *                      on success, NULL otherwise.
 */
extern struct gnl_socket_service_connection *gnl_socket_service_connect(const char *socket_name);

/**
 * Close a connection to the given connection.
 *
 * @param connection    The socket_service_connection instance.
 * @param socket_name   The name of the socket where to disconnect.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
extern int gnl_socket_service_close(struct gnl_socket_service_connection *connection);

/**
 * Send a message through the given connection.
 *
 * @param connection    The socket_service_connection instance.
 * @param message       The message to send.
 *
 * @return              Returns the number of bytes sent on success,
 *                      -1 otherwise.
 */
extern int gnl_socket_service_emit(const struct gnl_socket_service_connection *connection, const char *message);

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
extern int gnl_socket_service_read(const struct gnl_socket_service_connection *connection, char **message, int size);

#endif //GNL_SOCKET_SERVICE_H