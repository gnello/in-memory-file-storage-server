
#ifndef GNL_SOCKET_SERVICE_H
#define GNL_SOCKET_SERVICE_H

/**
 * Connect to the given socket name.
 *
 * @param socket_name   The name of the socket where to connect.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
int gnl_socket_service_connect(const char *socket_name);

#endif //GNL_SOCKET_SERVICE_H