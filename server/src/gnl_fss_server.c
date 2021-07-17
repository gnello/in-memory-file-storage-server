#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <gnl_logger.h>
#include <gnl_socket_request.h>
#include "gnl_fss_thread_pool.c"
#include <gnl_macro_beg.h>

#define N 100

/**
 * Create the thread pool to handle clients requests.
 *
 * @param size          The size of the thread pool.
 * @param logger        The logger instance to use for logging.
 *
 * @return              Returns a thread pool struct on success,
 *                      NULL otherwise.
 */
static struct gnl_fss_thread_pool *create_thread_pool(int size, const struct gnl_logger *logger) {
    int res = gnl_logger_debug(logger, "creating the thread pool with %d workers...", size);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // create the working config
    struct gnl_fss_thread_pool *thread_pool = gnl_fss_thread_pool_init(size);
    GNL_NULL_CHECK(thread_pool, ENOMEM, NULL)

    res = gnl_logger_info(logger, "created the thread pool with %d workers", size);
    GNL_MINUS1_CHECK(res, errno, NULL)

    return thread_pool;
}

/**
 * Create the server using the given socket name.
 *
 * @param socket_name   The socket path to use to create the server.
 * @param logger        The logger instance to use for logging.
 *
 * @return              Return the server file descriptor on success,
 *                      -1 otherwise.
 */
static int create_server(const char *socket_name, const struct gnl_logger *logger) {
    int res;
    int fd_skt;

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

    return fd_skt;
}

/**
 * Run the server handling new connections or requests.
 *
 * @param fd_skt    The server file descriptor.
 * @param logger    The logger instance to use for logging.
 *
 * @return          Returns -1 on error, otherwise it never returns.
 */
static int run_server(int fd_skt, const struct gnl_logger *logger) {
    int res;

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

    // max active file descriptor index
    int fd_num = 0;

    // active file descriptors set
    fd_set set;

    // reset mask
    FD_ZERO(&set);

    // put the server file descriptor into the active file descriptors set
    FD_SET(fd_skt,&set);

    // update fd_num with the max file descriptor active index
    if (fd_skt > fd_num) {
        fd_num = fd_skt;
    }

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

                    // put the client file descriptor into the active file descriptors set
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
                            gnl_logger_error(logger, "invalid request of client %d: %s", fd_c, strerror(errno));

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
}

//TODO: add doc (in a header?)
int gnl_fss_server_start(struct gnl_fss_config *config) {
    // validate the socket name
    char *socket_name = config->socket;

    if (socket_name == NULL) {
        errno = EINVAL;

        return -1;
    }

    // instantiate the logger
    struct gnl_logger *logger;
    logger = gnl_logger_init(config->log_filepath, "gnl_fss_server", config->log_level);
    GNL_NULL_CHECK(logger, errno, -1)

    // start the thread pool
    struct gnl_fss_thread_pool *thread_pool = create_thread_pool(config->thread_workers, logger);
    if (thread_pool == NULL) {
        gnl_logger_error(logger, "error creating the thread pool: %s", strerror(errno));

        return -1;
    }

    // socket connection file descriptor
    int fd_skt;

    // create the server
    fd_skt = create_server(socket_name, logger);
    if (fd_skt == -1) {
        gnl_logger_error(logger, "error creating the server: %s", strerror(errno));

        // destroy the thread pool
        gnl_fss_thread_pool_destroy(thread_pool);

        // destroy the logger
        gnl_logger_destroy(logger);

        return -1;
    }

    // run the server
    int res = run_server(fd_skt, logger);
    if (res == -1) {
        gnl_logger_error(logger, "error running the server: %s", strerror(errno));

        return -1;
    }

    return 0;
}



// TODO: clean up in a signal handler
//    gnl_logger_destroy(logger);
//    close(fd_skt);
//    remove(socket_name);

#undef N

#include <gnl_macro_end.h>