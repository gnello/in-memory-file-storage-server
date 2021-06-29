
#ifndef GNL_SOCKET_CONNECTION_H
#define GNL_SOCKET_CONNECTION_H

/**
 * Socket connection instance.
 */
struct gnl_socket_connection {
    int fd; // the identifier of the socket connection
    char* socket_name; // the socket name
    int active; // if 1 the connection is active, otherwise (active == 0) is not.
};

#endif //GNL_SOCKET_CONNECTION_H