#define __USE_POSIX199309
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <errno.h>
#include <libgen.h>
#include <gnl_queue_t.h>
#include "../include/gnl_opt_handler.h"

#define GNL_SHORT_OPTS ":hf:w:W:D:r:R::d:t:l:u:c:p"
#define GNL_PRINTF_H_INITIAL_SPACE "  "
#define GNL_PRINTF_H_TAB "%-28s"
#define GNL_THROW_OPT_EXCEPTION(opt, error, message) {  \
    errno = EINVAL;                                     \
    if (opt != NULL) {                                  \
        *opt = optopt;                                  \
        *error = message;                               \
    }                                                   \
    return -1;                                          \
}

extern int errno;
extern char *optarg;
extern int optopt;

struct gnl_opt_handler {
    gnl_queue_t *command_queue;
    char *socket_filename;
    int debug;
};

struct gnl_opt_handler_el {
    char opt;
    void *arg;
};

/**
 * Print the two arguments in two columns.
 *
 * @param col1_string   The firs column value.
 * @param col2_string   The second column value.
 *
 * @return int          Returns always 0.
 */
static int printInTable(char *col1_string, char *col2_string) {
    printf(GNL_PRINTF_H_INITIAL_SPACE GNL_PRINTF_H_TAB "%s", col1_string, col2_string);

    return 0;
}

/**
 * Wait time milliseconds.
 *
 * @return  Returns the nanosleep result.
 */
static int wait_microseconds(time) {
    struct timespec ts;

    ts.tv_sec = time / 1000;
    ts.tv_nsec = (time % 1000) * 1000000;

    return nanosleep(&ts, NULL);
}

static int print_usage(const char* program_name) { //7
    printf("Usage: %s [options]\n", program_name);
    printf("Write and read files to and from the In Memory Storage Server.\n");
    printf("Example: %s -r file1 -d /dev/null -w ./mydir\n", program_name);
    printf("\n");
    printf("The order of the options matters, the options will be handled in the order\n");
    printf("they appear.\n");
    printf("The -f, -h, -p options can not be specified more than once.\n");
    printf("\n");

    printInTable("-h", "Print this message and exit.\n");
    printInTable("-f FILENAME", "Connect to the FILENAME socket.\n");

    printInTable("-w DIRNAME[,N=0]", "Send DIRNAME files to the Server.\n");
    printInTable("", "If N is specified, send N files of DIRNAME\n");
    printInTable("", "to the Server.\n");

    printInTable("-W FILE1[,FILE2...]", "Send any given FILE to the Server.\n");

    printInTable("-D DIRNAME", "Store the files trashed by the Server following\n");
    printInTable("", "a -w or -W option into DIRNAME.\n");

    printInTable("-r FILE1[,FILE2...]", "Read FILE from the Server.\n");

    printInTable("-R [N=0]", "Read all files stored on the Server.\n");
    printInTable("", "If N is specified, read N random files from\n");
    printInTable("", "the Server.\n");

    printInTable("-d DIRNAME", "Store the files read from the Server following\n");
    printInTable("", "a -r or -R option into DIRNAME.\n");

    printInTable("-t TIME", "Wait TIME milliseconds between sequential requests\n");
    printInTable("", "to the Server.\n");

    printInTable("-l FILE1[,FILE2...]", "Acquire the lock on FILE.\n");
    printInTable("-u FILE1[,FILE2...]", "Release the lock on FILE.\n");

    printInTable("-c FILE1[,FILE2...]", "Remove FILE from the Server.\n");
    printInTable("-p", "Print the log of the requests made to the server.\n");

    return 0;
}

//
//static int arg_f(const char* param) { //3
//    return 0;
//}
//
//static int arg_w(const char* param) { //11
//    return 0;
//}
//
//static int arg_W(const char* param) { //10
//    return 0;
//}
//
//static int arg_D(const char* param) { //0
//    return 0;
//}
//
//static int arg_r(const char* param) { //6
//    return 0;
//}
//
//static int arg_R(const char* param) { //5
//    return 0;
//}
//
//static int arg_d(const char* param) { //1
//    return 0;
//}
//
//static int arg_t(const char* param) { //8
//    return 0;
//}
//
//static int arg_l(const char* param) { //11
//    return 0;
//}
//
//static int arg_u(const char* param) { //9
//    return 0;
//}
//
//static int arg_c(const char* param) { //0
//    return 0;
//}
//
//static int arg_p(const char* param) { //4
//    return 0;
//}

/**
 * Print the handler instance.
 *
 * @param handler   The handler to be printed;
 */
void printQueue(struct gnl_opt_handler *handler) {
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
                    GNL_THROW_OPT_EXCEPTION(opt_err, error, "option repetition")
                }

                handler->debug = 1;
                break;

            // set the socket filename
            case 'f':
                if (handler->socket_filename != NULL) {
                    GNL_THROW_OPT_EXCEPTION(opt_err, error, "option already encountered")
                }

                handler->socket_filename = optarg;
                break;

            // print the usage message
            case 'h':
                // basename removes path information
                // POSIX version, can modify the argument.
                print_usage(basename(argv[0]));
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

            // put every other valid option in the command queue
            default:
                opt_el = (struct gnl_opt_handler_el *)malloc(argc * sizeof(struct gnl_opt_handler_el));
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

void gnl_opt_handler_destroy(gnl_opt_handler *handler) {
    gnl_queue_destroy(handler->command_queue, free);
    free(handler);
}

int gnl_opt_handler_handle(gnl_opt_handler *handler) {
    int time = 0;

    struct gnl_opt_handler_el el;
    struct gnl_opt_handler_el previous_el;
    void *raw_el;
    char *ptr = NULL;

    while ((raw_el = gnl_queue_dequeue(handler->command_queue)) != NULL) {
        el = *(struct gnl_opt_handler_el *)raw_el;

        switch (el.opt) {
            // update the requests delay
            case 't':
                time = strtol(el.arg, &ptr, 10);

                // if no digits found
                if ((char *)el.arg == ptr) {
                    errno = EINVAL;

                    return -1;
                }

                // if there was an error
                if (errno != 0) {
                    return -1;
                }

                // process next command immediately
                continue;
                /* NOT REACHED */
                break;
        }

        printf("command: %c %s\n", el.opt, (char *)el.arg);

        previous_el = el;
        free(raw_el);

        wait_microseconds(time);
    }

    return 0;
}

#undef GNL_SHORT_OPTS
#undef GNL_PRINTF_H_INITIAL_SPACE
#undef GNL_PRINTF_H_TAB
#undef GNL_THROW_OPT_EXCEPTION