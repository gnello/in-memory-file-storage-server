
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

#endif //GNL_SOCKET_SERVICE_H