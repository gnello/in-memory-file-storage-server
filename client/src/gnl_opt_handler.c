#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <errno.h>
#include <libgen.h>
#include <gnl_queue_t.h>
#include "../include/gnl_opt_handler.h"
#include "./gnl_opt_arg.c"
#include <gnl_macro_beg.h>

#define GNL_SHORT_OPTS ":hf:w:W:D:r:R::d:t:l:u:c:p"

#define GNL_THROW_OPT_EXCEPTION(opt, error, message) {  \
    errno = EINVAL;                                     \
    if ((opt) != NULL) {                                  \
        *(opt) = optopt;                                  \
        *(error) = message;                               \
    }                                                   \
    return -1;                                          \
}

extern int errno;

struct gnl_opt_handler {
    struct gnl_queue_t *command_queue;
    char *socket_filename;
    int debug;
};

struct gnl_opt_handler_el {
    int opt;
    void *arg;
};

int nanosleep(const struct timespec *req, struct timespec *rem);

/**
 * Wait time milliseconds.
 *
  * @param time The seconds to wait.
 *
 * @return  Returns the nanosleep result.
 */
static int wait_microseconds(int time) {
    struct timespec ts;

    ts.tv_sec = time / 1000;
    ts.tv_nsec = (time % 1000) * 1000000;

    return nanosleep(&ts, NULL);
}

/**
 * Print the handler instance.
 *
 * @param handler   The handler to be printed;
 */
static void print_queue(struct gnl_opt_handler *handler) {
    struct gnl_opt_handler_el el;
    void *el_void;

    printf("socket_filename: %s\n", handler->socket_filename);
    printf("debug: %d\n", handler->debug);

    while ((el_void = gnl_queue_dequeue(handler->command_queue)) != NULL) {
        el = *(struct gnl_opt_handler_el *)el_void;

        printf("command: %c %s\n", el.opt, (char *)el.arg);
        free(el_void);
    }
}

struct gnl_opt_handler *gnl_opt_handler_init() {
    struct gnl_opt_handler *handler = (struct gnl_opt_handler *)malloc(sizeof(struct gnl_opt_handler));

    if (handler == NULL) {
        errno = ENOMEM;

        return NULL;
    }

    // initialize struct
    handler->debug = 0;
    handler->socket_filename = NULL;
    handler->command_queue = gnl_queue_init();

    if (handler->command_queue == NULL) {
        errno = ENOMEM;
        gnl_opt_handler_destroy(handler);

        return NULL;
    }

    return handler;
}

void gnl_opt_handler_destroy(struct gnl_opt_handler *handler) {
    gnl_queue_destroy(handler->command_queue, free);
    free(handler);
}

int gnl_opt_handler_parse_opt(struct gnl_opt_handler *handler, int argc, char* argv[], char *opt_err, char **error) {
    int opt;
    struct gnl_opt_handler_el *opt_el;
    int res;

    // start arguments parse
    int i = 0;
    while ((opt = getopt(argc, argv, GNL_SHORT_OPTS)) != -1) {
        // the -f, -h, -p options can not be specified more than once
        switch (opt) {
            // enable operations log print
            case 'p':
                if (handler->debug != 0) {
                    optopt = 'p';

                    GNL_THROW_OPT_EXCEPTION(opt_err, error, "option repeated")
                }

                handler->debug = 1;
                break;

            // set the socket filename
            case 'f':
                if (handler->socket_filename != NULL) {
                    optopt = 'f';
                    GNL_THROW_OPT_EXCEPTION(opt_err, error, "option repeated")
                }

                handler->socket_filename = optarg;
                break;

            // print the usage message
            case 'h':
                // basename removes path information
                // POSIX version, can modify the argument.
                arg_h(basename(argv[0]));
                gnl_opt_handler_destroy(handler);

                exit(EXIT_FAILURE);
                /* NOT REACHED */
                break;

            // option not valid
            case ':':
                GNL_THROW_OPT_EXCEPTION(opt_err, error, "argument required")
                /* NOT REACHED */
                break;

            // option not valid
            case '?':
                GNL_THROW_OPT_EXCEPTION(opt_err, error, "invalid option")
                /* NOT REACHED */
                break;

            // put every other valid option into the command queue
            default:
                opt_el = (struct gnl_opt_handler_el *)malloc(sizeof(struct gnl_opt_handler_el));
                opt_el->opt = opt;
                opt_el->arg = optarg;
                res = gnl_queue_enqueue(handler->command_queue, (void *)opt_el);

                if (res == -1) {
                    errno = ENOMEM;
                    gnl_opt_handler_destroy(handler);

                    return -1;
                }

                i++;
                break;
        }
    }

    return 0;
}

int gnl_opt_handler_handle(struct gnl_opt_handler *handler) {
    int opt_t_value = 0;
    int res;

    // first open the connection to the server
    res = arg_f_start(handler->socket_filename);
    GNL_MINUS1_CHECK(res, errno, -1);

    struct gnl_opt_handler_el *el;
    struct gnl_opt_handler_el *previous_el = NULL;

    while ((el = (struct gnl_opt_handler_el *)gnl_queue_dequeue(handler->command_queue)) != NULL) {

        switch (el->opt) {
            // update the requests delay
            case 't':
                GNL_TO_INT(opt_t_value, el->arg, -1)

                free(el);
                // process next command immediately
                continue;
                /* NOT REACHED */
                break;

            case 'w':
                if (previous_el->opt == 'D') {
                    res = arg_w(el->arg, previous_el->arg);
                } else {
                    res = arg_w(el->arg, NULL);
                }

                GNL_MINUS1_CHECK(res, errno, -1);
                break;
        }

        previous_el = el;

        free(el);
        wait_microseconds(opt_t_value);
    }

    free(previous_el);

    // at the end close the connection to the server
    res = arg_f_end(handler->socket_filename);
    GNL_MINUS1_CHECK(res, errno, -1);

    return 0;
}

#undef GNL_SHORT_OPTS
#undef GNL_THROW_OPT_EXCEPTION

#include <gnl_macro_end.h>