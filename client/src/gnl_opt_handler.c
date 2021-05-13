#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <libgen.h>
#include <gnl_queue_t.h>
#include "../include/gnl_opt_handler.h"

#define SHORT_OPTS "hf:w:W:D:r:R::d:t:l:u:c:p"
#define PRINTF_H_INITIAL_SPACE "  "
#define PRINTF_H_TAB "%-28s"

extern int errno;
extern char *optarg;
extern int opterr, optind;

struct gnl_opt_handler {
    gnl_queue_t *command_queue;
    char *socket_filename;
    int debug;
};

struct gnl_opt_handler_el {
    char opt;
    void *arg;
};

static int handle_arg_h(const char* program_name) { //7
    printf("Usage: %s [options]\n", program_name);
    printf("Write and read files to and from the In Memory Storage Server.\n");
    printf("Example: %s -r file1 -d /dev/null -w ./mydir\n", program_name);
    printf("\n");
    printf("The order of the options matters, the options will be handled in the order\n");
    printf("they appear.\n");
    printf("\n");

    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Print this message and exit.\n", "-h");
    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Connect to the FILENAME socket.\n", "-f FILENAME");

    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Send DIRNAME files to the Server.\n", "-w DIRNAME[,N=0]");
    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "If N is specified, send N files of DIRNAME\n", "");
    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "to the Server.\n", "");

    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Send any given FILE to the Server.\n", "-W FILE1[,FILE2...]");

    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Store the files trashed by the Server following\n", "-D DIRNAME");
    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "a -w or -W option into DIRNAME.\n", "");

    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Read FILE from the Server.\n", "-r FILE1[,FILE2...]");

    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Read all files stored on the Server.\n", "-R [N=0]");
    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "If N is specified, read N random files from\n", "");
    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "the Server.\n", "");

    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Store the files read from the Server following\n", "-d DIRNAME");
    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "a -r or -R option into DIRNAME.\n", "");

    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Wait TIME milliseconds between sequential requests\n", "-t TIME");
    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "to the Server.\n", "");

    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Acquire the lock on FILE.\n", "-l FILE1[,FILE2...]");
    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Release the lock on FILE.\n", "-u FILE1[,FILE2...]");

    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Remove FILE from the Server.\n", "-c FILE1[,FILE2...]");
    printf(PRINTF_H_INITIAL_SPACE PRINTF_H_TAB "Print the log of the requests made to the server.\n", "-p");

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

struct gnl_opt_handler *gnl_opt_handler_init(int argc, char* argv[]) {
    struct gnl_opt_handler *opt_handler = (struct gnl_opt_handler *)malloc(sizeof(struct gnl_opt_handler));

    // initialize struct
    opt_handler->debug = 0;
    opt_handler->socket_filename = NULL;
    opt_handler->command_queue = NULL;

    int opt;
    struct gnl_opt_handler_el *opt_el;
    int res;

    // start arguments parse
    int i=0;
    while ((opt = getopt(argc, argv, SHORT_OPTS)) != -1) {
        //TODO: controllare che -f, -h e -p non siano ripetuti
        switch (opt) {
            case 'p':
                opt_handler->debug = 1;
                break;

            case 'f':
                //TODO: controllare che non serva strcpy
                opt_handler->socket_filename = optarg;
                break;

            case 'h':
                // basename removes path information.
                // POSIX version, can modify the argument.
                handle_arg_h(basename(argv[0]));
                gnl_opt_handler_destroy(opt_handler);

                exit(EXIT_FAILURE);
                /* NOT REACHED */
                break;

            case '?':
                return NULL;

            default:
                // initialize the command queue
                if (opt_handler->command_queue == NULL) {
                    opt_handler->command_queue = gnl_queue_init();
                    if (opt_handler->command_queue == NULL) {
                        errno = ENOMEM;
                        gnl_opt_handler_destroy(opt_handler);

                        return NULL;
                    }
                }

                opt_el = (struct gnl_opt_handler_el *)malloc(argc * sizeof(struct gnl_opt_handler_el));
                opt_el->opt = opt;
                opt_el->arg = optarg;
                res = gnl_queue_enqueue(opt_handler->command_queue, (void *)opt_el);

                if (res == -1) {
                    errno = ENOMEM;
                    gnl_opt_handler_destroy(opt_handler);

                    return NULL;
                }

                i++;
                break;
        }
    }

    return opt_handler;
}

void gnl_opt_handler_destroy(gnl_opt_handler *handler) {
    gnl_queue_destroy(handler->command_queue, free);
    free(handler);
}

int gnl_opt_handler_handle(gnl_opt_handler *handler) {
    //TODO: handle queue
    //TODO: il timer meglio gestirlo con una sleep o con una signal alarm?

    int time = 0;

    struct gnl_opt_handler_el el;
    struct gnl_opt_handler_el previous_el;
    void *raw_el;
    char *ptr = NULL;

    while ((raw_el = gnl_queue_dequeue(handler->command_queue)) != NULL) {
        el = *(struct gnl_opt_handler_el *)raw_el;

        // update the requests delay if it is necessary
        if (el.opt == 't') {
            //TODO: è giusto fare qui la conversione?
            time = strtol(el.arg, &ptr, 10); //TODO: https://stackoverflow.com/questions/26080829/detecting-strtol-failure/26083517
        }

        printf("command: %c %s\n", el.opt, (char *)el.arg);

        previous_el = el;
        free(raw_el);
    }

    return 0;
}

#undef SHORT_OPTS
#undef PRINTF_H_INITIAL_SPACE
#undef PRINTF_H_TAB