
#ifndef GNL_FSS_SOCKET_SERVICE_H
#define GNL_FSS_SOCKET_SERVICE_H

/**
 * Connect to the given socket name.
 *
 * @param socket_name   The name of the socket where to connect.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
int gnl_fss_socket_service_connect(const char *socket_name);

#endif //GNL_FSS_SOCKET_SERVICE_H