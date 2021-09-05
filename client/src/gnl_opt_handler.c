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
    int prints;
};

struct gnl_opt_handler_el {
    int opt;
    void *arg;
};

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

struct gnl_opt_handler *gnl_opt_handler_init() {
    struct gnl_opt_handler *handler = (struct gnl_opt_handler *)malloc(sizeof(struct gnl_opt_handler));

    if (handler == NULL) {
        errno = ENOMEM;

        return NULL;
    }

    // initialize struct
    handler->prints = 0;
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
                if (handler->prints != 0) {
                    optopt = 'p';

                    GNL_THROW_OPT_EXCEPTION(opt_err, error, "option repeated")
                }

                handler->prints = 1;
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

            // option not valid
            case ':':
                GNL_THROW_OPT_EXCEPTION(opt_err, error, "argument required")
                /* NOT REACHED */

            // option not valid
            case '?':
                GNL_THROW_OPT_EXCEPTION(opt_err, error, "invalid option")
                /* NOT REACHED */

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
    int arg_f;
    int res = 0;

    // check if we have to enable output
    if (handler->prints) {
        enable_output();
    }

    // first open the connection to the server
    arg_f = arg_f_start(handler->socket_filename);
    GNL_MINUS1_CHECK(arg_f, errno, -1);

    struct gnl_opt_handler_el *el;
    struct gnl_opt_handler_el previous_el = {0, NULL};

    while ((el = (struct gnl_opt_handler_el *)gnl_queue_dequeue(handler->command_queue)) != NULL) {

        switch (el->opt) {
            // update the requests delay
            case 't':
                GNL_TO_INT(opt_t_value, el->arg, -1)

                free(el);
                // process next command immediately
                continue;
                /* NOT REACHED */

            case 'w':
                if (previous_el.opt == 'D') {
                    res = arg_w(el->arg, previous_el.arg);
                } else {
                    res = arg_w(el->arg, NULL);
                }
                break;

            case 'W':
                if (previous_el.opt == 'D') {
                    res = arg_W(el->arg, previous_el.arg);
                } else {
                    res = arg_W(el->arg, NULL);
                }
                break;

            case 'c':
                res = arg_c(el->arg);
                break;

            case 'l':
                res = arg_l(el->arg);
                break;

            case 'r':
                if (previous_el.opt == 'd') {
                    res = arg_r(el->arg, previous_el.arg);
                } else {
                    res = arg_r(el->arg, NULL);
                }
                break;

            case 'u':
                res = arg_u(el->arg);
                break;
        }

        previous_el = *el;

        free(el);

        // if an error happen stop the execution
        if (res == -1) {
            break;
        }

        wait_microseconds(opt_t_value);
    }

    // at the end close the connection to the server
    arg_f = arg_f_end(handler->socket_filename);
    GNL_MINUS1_CHECK(arg_f, errno, -1);

    return res;
}

#undef GNL_SHORT_OPTS
#undef GNL_THROW_OPT_EXCEPTION

#include <gnl_macro_end.h>