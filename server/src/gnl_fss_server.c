#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <gnl_logger.h>
#include <gnl_socket_request.h>
#include <gnl_macro_beg.h>

#define N 100

struct gnl_logger *logger;

int gnl_fss_server_start(gnl_fss_config *config) {
    char *socket_name = config->socket;

    if (socket_name == NULL) {
        errno = EINVAL;

        return -1;
    }

    // instantiate the logger
    logger = gnl_logger_init(config->log_filepath, "gnl_fss_server", config->log_level);
    GNL_NULL_CHECK(logger, errno, -1)

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
    long nread;

    res = gnl_logger_debug(logger, "server is starting...");
    GNL_MINUS1_CHECK(res, errno, -1)

    // create socket address
    struct sockaddr_un sa;
    strncpy(sa.sun_path, socket_name, strlen(socket_name) + 1);
    sa.sun_family = AF_UNIX;

    // create the server
    fd_skt = socket(AF_UNIX, SOCK_STREAM, 0);
    GNL_MINUS1_CHECK(fd_skt, errno, -1)

    // check first access of the file log
    res = gnl_logger_debug(logger, "socket created with id %d", fd_skt);
    GNL_MINUS1_CHECK(res, errno, -1)

    // bind the address
    res = bind(fd_skt, (struct sockaddr *)&sa, sizeof(sa));
    GNL_MINUS1_CHECK(res, errno, -1)

    gnl_logger_debug(logger, "socket %d binded to address %s", fd_skt, socket_name);

    // listen
    res = listen(fd_skt, SOMAXCONN);
    GNL_MINUS1_CHECK(res, errno, -1)

    gnl_logger_info(logger, "server ready, listening for connections");

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

                    gnl_logger_debug(logger, "client %d requested to connect, accepted", fd_c);

                    // update the active file descriptors set
                    FD_SET(fd_c, &set);

                    // update fd_num with the max file descriptor active index
                    if (fd_c > fd_num) {
                        fd_num = fd_c;
                    }

                } else { // if there is an I/O request...

                    // clear the buffer
                    memset(buf, 0, N);

                    // read data
                    nread = read(fd, buf, N);
                    GNL_MINUS1_CHECK(nread, errno, -1)

                    // if EOF...
                    if (nread == 0) {
                        // remove fd from the active file descriptors set
                        FD_CLR(fd, &set);

                        // decrease the max active file descriptor index by one
                        if (fd == fd_num) {
                            fd_num--;
                        }

                        // close the current file descriptor
                        gnl_logger_debug(logger, "client %d gone away, closing connection", fd_c);

                        res = close(fd);
                        GNL_MINUS1_CHECK(res, errno, -1)
                    } else {
                        // do something with the buffer...
                        gnl_logger_debug(logger, "client %d sent a message, decoding request...", fd_c);

                        // decode request
                        struct gnl_socket_request *request;
                        request = gnl_socket_request_read(buf);

                        if (request == NULL) {
                            gnl_logger_error(logger, "Invalid request of client %d: %s", fd_c, strerror(errno));

                            // resume loop
                            continue;
                        }

                        char *request_type;
                        res = gnl_socket_request_to_string(request, &request_type);
                        GNL_MINUS1_CHECK(res, errno, -1)

                        gnl_logger_debug(logger, "client %d sent a request: %s", fd_c, request_type);
                    }
                }
            }
        }
    }

    // clean up
    gnl_logger_destroy(logger);
    close(fd_skt);
    remove(socket_name);

    return 0;
}

#undef N

#include <gnl_macro_end.h>