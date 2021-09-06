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

#define GNL_THROW_OPT_EXCEPTION(opt, error, message) {                                                                  \
    errno = EINVAL;                                                                                                     \
    if ((opt) != NULL) {                                                                                                \
        *(opt) = optopt;                                                                                                \
        *(error) = message;                                                                                             \
    }                                                                                                                   \
    return -1;                                                                                                          \
}

/**
 * {@inheritDoc}
 */
extern int errno;

/**
 * {@inheritDoc}
 */
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
 * Wait for the given milliseconds.
 *
 * @param time The microseconds to wait.
 *
 * @return  Returns 0 on success, -1 otherwise.
 */
static int wait_microseconds(int time) {
    struct timespec ts;

    ts.tv_sec = time / 1000;
    ts.tv_nsec = (time % 1000) * 1000000;

    return nanosleep(&ts, NULL);
}

/**
 * {@inheritDoc}
 */
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

/**
 * {@inheritDoc}
 */
void gnl_opt_handler_destroy(struct gnl_opt_handler *handler) {
    gnl_queue_destroy(handler->command_queue, free);
    free(handler);
}

/**
 * {@inheritDoc}
 */
int gnl_opt_handler_parse_opt(struct gnl_opt_handler *handler, int argc, char* argv[], char *opt_err, char **error) {
    int opt;
    struct gnl_opt_handler_el *opt_el;
    int res;

    // start arguments parse
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

                // if the -R option argument is apparently NULL...
                // this case is normal due to the optional argument definition,
                // see "man 3 getopt" for details
                if (opt_el->opt == 'R' && optarg == NULL) {

                    // if the argv[optind] contains the -R argument...
                    if (optind < argc && argv[optind] != NULL && argv[optind][0] != '\0' && argv[optind][0] != '-') {

                        // get the argument
                        opt_el->arg = argv[optind];

                        // increase the opt index
                        optind++;
                    }
                    // if the argv[optind] is not a valid argument for the -R option...
                    else {
                        // set the argument to default (0)
                        opt_el->arg = "0";
                    }
                }

                res = gnl_queue_enqueue(handler->command_queue, (void *)opt_el);

                if (res == -1) {
                    // let the errno bubble

                    gnl_opt_handler_destroy(handler);

                    return -1;
                }
                break;
        }
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
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

    // the command to execute
    struct gnl_opt_handler_el command = {0, NULL};

    // the next command
    struct gnl_opt_handler_el *next_command = NULL;

    char *D_arg;
    char *d_arg;

    // run the command and fetch the next one. The first iteration
    // will have no effect since the initial command is not already
    // assigned.
    do {

        // free memory
        if (next_command != NULL) {
            free(next_command);
        }

        // get the next command
        next_command = (struct gnl_opt_handler_el *)gnl_queue_dequeue(handler->command_queue);

        // store the -D argument to use within -w or -W options
        D_arg = NULL;
        if (next_command != NULL && next_command->opt == 'D') {
            D_arg = next_command->arg;
        }

        // store the -d argument to use within -r or -R options
        d_arg = NULL;
        if (next_command != NULL && next_command->opt == 'd') {
            d_arg = next_command->arg;
        }

        // execute the command
        switch (command.opt) {
            // update the requests delay
            case 't':
                GNL_TO_INT(opt_t_value, command.arg, -1)

                free(next_command);
                // process next command immediately
                continue;
                /* NOT REACHED */

            case 'w':
                res = arg_w(command.arg, D_arg);
                break;

            case 'W':
                res = arg_W(command.arg, D_arg);
                break;

            case 'c':
                res = arg_c(command.arg);
                break;

            case 'l':
                res = arg_l(command.arg);
                break;

            case 'r':
                res = arg_r(command.arg, d_arg);
                break;

            case 'R':
                res = arg_R(command.arg, d_arg);
                break;

            case 'u':
                res = arg_u(command.arg);
                break;
        }

        if (next_command != NULL) {
            command = *next_command;
        }

        // if an error happen stop the execution
        if (res == -1) {
            break;
        }

        wait_microseconds(opt_t_value);

    } while (next_command != NULL);

    // at the end close the connection to the server
    arg_f = arg_f_end(handler->socket_filename);
    GNL_MINUS1_CHECK(arg_f, errno, -1);

    return res;
}

#undef GNL_SHORT_OPTS
#undef GNL_THROW_OPT_EXCEPTION

#include <gnl_macro_end.h>