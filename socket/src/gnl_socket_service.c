#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "../include/gnl_socket_service.h"
#include <gnl_macro_beg.h>

int gnl_socket_service_connect(const char *socket_name) {
    int fd_skt;

    // create the socket address
    struct sockaddr_un sa;
    strncpy(sa.sun_path, socket_name, strlen(socket_name) + 1);
    sa.sun_family = AF_UNIX;

    // create the socket and get his file descriptor
    fd_skt = socket(AF_UNIX,SOCK_STREAM,0);
    GNL_MINUS1_CHECK(fd_skt, errno, -1);

    // connect to the socket
    int res = connect(fd_skt, (struct sockaddr *)&sa, sizeof(sa));
    GNL_MINUS1_CHECK(res, errno, -1);

    return 0;
}

#include <gnl_macro_end.h>