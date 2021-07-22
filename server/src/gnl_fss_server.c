#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <signal.h>
#include <bits/sigaction.h>
#include <gnl_logger.h>
#include "gnl_fss_thread_pool.c"
#include "../include/gnl_fss_server.h"
#include <gnl_macro_beg.h>

#define GNL_FSS_SERVER_BUFFER_LEN 100

volatile sig_atomic_t soft_termination = 0;
volatile sig_atomic_t hard_termination = 0;

void termination_signal_handler(int signal) {
    if (signal == SIGHUP) {
        soft_termination = 1;
    } else {
        hard_termination = 1;
    }
}

static int handle_signals() {
    int res;
    sigset_t set;

    struct sigaction sa;
    struct sigaction ignore_sa;

    // if a client goes away do not allow the server to terminate ignoring SIGPIPE
    memset(&ignore_sa, 0, sizeof(ignore_sa));
    ignore_sa.sa_handler = SIG_IGN;

    // automatically restart interrupted system calls
    ignore_sa.sa_flags = SA_RESTART;

    res = sigaction(SIGPIPE, &ignore_sa, NULL);
    GNL_MINUS1_CHECK(res, errno, -1);

    // install the termination signal handler
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = termination_signal_handler;

    // automatically restart interrupted system calls
    sa.sa_flags = SA_RESTART;

    // reset the handler mask
    sigemptyset(&set);

    // block the following signals
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGHUP);
    sa.sa_mask = set;

    // install the signal handler for the following signals
    res = sigaction(SIGINT, &sa, NULL);
    GNL_MINUS1_CHECK(res, errno, -1);

    res = sigaction(SIGQUIT, &sa, NULL);
    GNL_MINUS1_CHECK(res, errno, -1);

    res = sigaction(SIGHUP, &sa, NULL);
    GNL_MINUS1_CHECK(res, errno, -1);

    return 0;
}

/**
 * Create the thread pool to handle clients requests.
 *
 * @param size          The size of the thread pool.
 * @param config        The configuration instance of the server.
 * @param logger        The logger instance to use for logging.
 *
 * @return              Returns a thread pool struct on success,
 *                      NULL otherwise.
 */
static struct gnl_fss_thread_pool *create_thread_pool(int size, const struct gnl_fss_config *config, const struct gnl_logger *logger) {
    int res = gnl_logger_debug(logger, "creating the thread pool with %d workers...", size);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // create the working config
    struct gnl_fss_thread_pool *thread_pool = gnl_fss_thread_pool_init(size, config);
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

    gnl_logger_debug(logger, "socket %d binded to the address %s", fd_skt, socket_name);

    // listen
    res = listen(fd_skt, SOMAXCONN);
    GNL_MINUS1_CHECK(res, errno, -1)

    gnl_logger_info(logger, "server ready, listening for connections");

    return fd_skt;
}

/**
 * Run the server handling new connections or requests.
 *
 * @param fd_skt            The server file descriptor.
 * @param thread_pool       The tread pool were to dispatch the message.
 * @param logger            The logger instance to use for logging.
 *
 * @return                  Returns -1 on error, otherwise it never returns.
 */
static int run_server(int fd_skt, struct gnl_fss_thread_pool *thread_pool, const struct gnl_logger *logger) {
    int res;

    // active file descriptors waited for reading
    fd_set rdset;

    // file descriptor index to check SC select results
    int fd;

    // file descriptor of a client
    int fd_c;

    // read buffer
    char buf[GNL_FSS_SERVER_BUFFER_LEN];

    // number of chars read
    long nread;

    // max active file descriptor index
    int fd_num = 0;

    // active file descriptors set
    fd_set set;

    // active connections
    int active_connections = 0;

    // get the master channel of the thread pool to read a result from a worker thread.
    int master_channel = gnl_fss_thread_pool_master_channel(thread_pool);
    GNL_MINUS1_CHECK(master_channel, errno, -1)

    // reset mask
    FD_ZERO(&set);

    // put the server file descriptor into the active file descriptors set
    FD_SET(fd_skt,&set);

    // put the master_channel file descriptor into the active file descriptors set
    FD_SET(master_channel, &set);

    // update fd_num with the max file descriptor active index
    if (fd_skt > fd_num) {
        fd_num = fd_skt;
    }

    while (1) {

        // update active file descriptors waited for reading set
        rdset = set;

        // wait for connections
        res = select(fd_num + 1, &rdset, NULL, NULL, NULL);
        if (res == -1) {
            if (errno == EINTR && hard_termination == 1) {
                gnl_logger_info(logger, "hard termination: the server will shut down immediately, "
                                        "every active connection will be closed.");

                return 0;
            }

            if (errno == EINTR && soft_termination == 1) {
                gnl_logger_info(logger, "soft termination: the server will shut down after every clients "
                                        "request will be handled, no others connections will be accepted.");

                if (active_connections == 0) {
                    return 0;
                } else {
                    // wait for current active clients termination
                    continue;
                }
            }

            // if this point is reached then there is an error, return
            return -1;
        }

        gnl_logger_debug(logger, "select (system call) returned with success, handling");

        // foreach active file descriptor...
        for (fd=0; fd<=fd_num; fd++) {

            // if the current fd is ready to be read...
            if (FD_ISSET(fd, &rdset)) {

                // if there is an incoming connection...
                if (fd == fd_skt) {

                    gnl_logger_debug(logger, "a client requested to connect");

                    // accept the connection
                    fd_c = accept(fd_skt, NULL, 0);
                    GNL_MINUS1_CHECK(res, errno, -1)

                    // if a soft termination is in progress, refuse the connection
                    if (soft_termination == 1) {
                        res = close(fd_c);
                        GNL_MINUS1_CHECK(res, errno, -1)

                        gnl_logger_debug(logger, "soft termination in progress, connection from client refused", fd_c);

                        // resume for loop
                        continue;
                    }

                    gnl_logger_debug(logger, "connection from client accepted, assigned id %d", fd_c);

                    active_connections++;
                    gnl_logger_debug(logger, "the server has now %d active connections", active_connections);

                    // put the client file descriptor into the active file descriptors set
                    FD_SET(fd_c, &set);

                    // update fd_num with the max file descriptor active index
                    if (fd_c > fd_num) {
                        fd_num = fd_c;
                    }

                } else if (fd == master_channel) { // a worker has handled a request

                    gnl_logger_debug(logger, "received message from the thread pool");

                    // clear the buffer
                    memset(buf, 0, GNL_FSS_SERVER_BUFFER_LEN);

                    // read the client file descriptor from the channel
                    nread = read(fd, buf, GNL_FSS_SERVER_BUFFER_LEN);
                    if (nread == -1) {
                        gnl_logger_error(logger, "error reading the message: %s", strerror(errno));

                        // do not stop the server: the show must go on
                        continue;
                    }

                    // initialize the message struct sent by a worker
                    struct gnl_message_n *message_from_worker = gnl_message_n_init();
                    GNL_NULL_CHECK(message_from_worker, errno, -1)

                    // decode the message sent by a worker
                    res = gnl_message_n_read(buf, message_from_worker);
                    GNL_MINUS1_CHECK(res, errno, -1)

                    // finally get the file descriptor
                    fd_c = message_from_worker->number;

                    // the message_from_worker is not necessary anymore, destroy it
                    gnl_message_n_destroy(message_from_worker);

                    // if EOF...
                    if (fd_c == 0) {
                        active_connections--;

                        gnl_logger_debug(logger, "a client has gone away");
                        gnl_logger_debug(logger, "the server has now %d active connections", active_connections);

                        // resume for loop
                        continue;
                    }

                    gnl_logger_debug(logger, "client %d request handled", fd_c);

                    // put the client file descriptor back into the active file descriptors set
                    FD_SET(fd_c, &set);

                    // update fd_num with the max file descriptor active index
                    if (fd_c > fd_num) {
                        fd_num = fd_c;
                    }

                } else { // if there is an I/O request...

                    gnl_logger_debug(logger, "I/O request received from client %d", fd);

                    // remove the file descriptor from the active file descriptors set
                    FD_CLR(fd, &set);

                    // update fd_num
                    if (fd == fd_num) {
                        fd_num--;
                    }

                    // copy the file descriptor to prevent changes side effects
                    int locked_fd = fd;

                    // pass the file descriptor to the thread pool
                    res = gnl_fss_thread_pool_dispatch(thread_pool, (void *)&locked_fd);
                    GNL_MINUS1_CHECK(res, errno, -1)

                    gnl_logger_debug(logger, "I/O request sent to the thread pool");
                }
            }
        }

        gnl_logger_debug(logger, "select (system call) handling done, resume loop");
    }
}

int gnl_fss_server_start(const struct gnl_fss_config *config) {
    // validate the socket name
    char *socket_name = config->socket;

    if (socket_name == NULL) {
        errno = EINVAL;

        return -1;
    }

    // install the signal handler
    handle_signals();

    // instantiate the logger TODO: move in a thread
    struct gnl_logger *logger;
    logger = gnl_logger_init(config->log_filepath, "gnl_fss_server", config->log_level);
    GNL_NULL_CHECK(logger, errno, -1)

    gnl_logger_info(logger, "server is starting");

    // start the thread pool
    struct gnl_fss_thread_pool *thread_pool = create_thread_pool(config->thread_workers, config, logger);
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
    int res = run_server(fd_skt, thread_pool, logger);
    if (res == -1) {
        gnl_logger_error(logger, "error running the server: %s", strerror(errno));

        return -1;
    }

    // if you reach this point means that the server execution
    // is terminated (due to an error or a signal), so free memory
    gnl_fss_thread_pool_destroy(thread_pool);
    close(fd_skt);
    unlink(socket_name);

    gnl_logger_info(logger, "server has been shut down.");

    gnl_logger_destroy(logger);

    return 0;
}

#undef GNL_FSS_SERVER_BUFFER_LEN

#include <gnl_macro_end.h>