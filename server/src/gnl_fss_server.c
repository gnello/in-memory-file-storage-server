#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <gnl_macro_beg.h>

#define N 100

int gnl_fss_server_start(char *socket_name) {
    if (socket_name == NULL) {
        errno = EINVAL;

        return -1;
    }

    int res;

    // socket connection file descriptor
    int fd_skt;

    // max active file descriptor index
    int fd_num = 0;

    // active file descriptors set
    fd_set set;

    // active file descriptors waited for reading
    fd_set rdset;

    // file descriptor index to check SC select results
    int fd;

    // file descriptor of a client
    int fd_c;

    // read buffer
    char buf[N];

    // number of chars read
    int nread;

    // create socket address
    struct sockaddr_un sa;
    strncpy(sa.sun_path, socket_name, strlen(socket_name) + 1);
    sa.sun_family = AF_UNIX;

    // create the server
    fd_skt = socket(AF_UNIX, SOCK_STREAM, 0);
    GNL_MINUS1_CHECK(fd_skt, errno, -1)

    // bind the address
    res = bind(fd_skt, (struct sockaddr *)&sa, sizeof(sa));
    GNL_MINUS1_CHECK(res, errno, -1)

    // listen
    res = listen(fd_skt, SOMAXCONN);
    GNL_MINUS1_CHECK(res, errno, -1)

    // update fd_num with the max file descriptor active index
    if (fd_skt > fd_num) {
        fd_num = fd_skt;
    }

    // reset mask
    FD_ZERO(&set);
    FD_SET(fd_skt,&set);

    while (1) {

        // update active file descriptors waited for reading set
        rdset = set;

        // wait for connections
        res = select(fd_num + 1, &rdset, NULL, NULL, NULL);
        GNL_MINUS1_CHECK(res, errno, -1)

        // foreach active file descriptor...
        for (fd=0; fd<=fd_num; fd++) {

            // if the current fd is ready to be read...
            if (FD_ISSET(fd, &rdset)) {

                // if there is an incoming connection...
                if (fd == fd_skt) {

                    // accept the connection
                    fd_c = accept(fd_skt, NULL, 0);
                    GNL_MINUS1_CHECK(res, errno, -1)
printf("Accepted connection by client id %d on socket id %d\n", fd_c, fd_skt);
                    // update the active file descriptors set
                    FD_SET(fd_c, &set);

                    // update fd_num with the max file descriptor active index
                    if (fd_c > fd_num) {
                        fd_num = fd_c;
                    }

                } else { // if there is an I/O request...

                    // read data
                    nread = read(fd, buf, N);
                    GNL_MINUS1_CHECK(res, errno, -1)

                    // if EOF...
                    if (nread == 0) {
                        // remove fd from the active file descriptors set
                        FD_CLR(fd, &set);

                        // decrease the max active file descriptor index by one
                        if (fd == fd_num) {
                            fd_num--;
                        }

                        // close the current file descriptor
                        close(fd);
                        printf("Close connection of client id %d on socket id %d\n", fd_c, fd_skt);
                    } else {
                        // do something with the buffer...
                    }
                }
            }
        }
    }

    // clean up
    close(fd_skt);
    remove(socket_name);

    return 0;
}

#undef N

#include <gnl_macro_end.h>