#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <gnl_macro_beg.h>

#define SOCKET_NAME "./test.sk"
#define MESSAGE_CONNECTED "connected"
#define N 100

static int fd_skt;

int main() {
    int res;

    // create socket address
    struct sockaddr_un sa;
    strncpy(sa.sun_path, SOCKET_NAME, strlen(SOCKET_NAME) + 1);
    sa.sun_family = AF_UNIX;

    // create the server
    fd_skt = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_skt == -1) {
        perror("socket");
    }

    // bind the address
    while(bind(fd_skt, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
        if (errno == EADDRINUSE) {
            remove(SOCKET_NAME);
            continue;
        }

        perror("bind");
        exit(EXIT_FAILURE);
    }

    // listen
    res = listen(fd_skt, SOMAXCONN);
    if (res == -1) {
        perror("listen");
    }


    printf("socket service test started...");

    // accept connection
    int fd_c;

    while ((fd_c = accept(fd_skt, NULL, 0))) {
        write(fd_c, MESSAGE_CONNECTED, strlen(MESSAGE_CONNECTED) + 1);

        char buf[N];
        while (read(fd_c, buf, N) != 0) {
            write(fd_c, buf, strlen(buf) + 1);
        }

        close(fd_c);
    }

    close(fd_skt);
    remove(SOCKET_NAME);

    return 0;
}

#undef SOCKET_NAME
#undef MESSAGE_CONNECTED
#undef N

#include <gnl_macro_end.h>