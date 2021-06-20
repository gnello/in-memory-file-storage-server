#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include <errno.h>
#include <gnl_print_table.h>
#include "../include/gnl_fss_opt_handler.h"

#define GNL_SHORT_OPTS ":hf:"
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

/**
 * Print usage message.
 *
 * @param program_name  The name of the server main executable.
 */
static void print_usage(const char* program_name) { //7
    printf("Usage: %s [options]\n", program_name);
    printf("Start the In Memory Storage Server.\n");
    printf("Example: %s -f ./config.txt\n", program_name);
    printf("\n");

    gnl_print_table("-h", "Print this message and exit.\n");
    gnl_print_table("-f FILENAME", "Start the server with the FILENAME configuration file.\n");
}

int gnl_fss_opt_handler_parse(int argc, char* argv[], char **filepath, char *opt_err, char **error) {
    int opt;

    // start arguments parse
    while ((opt = getopt(argc, argv, GNL_SHORT_OPTS)) != -1) {
        // the -f, -h, -p options can not be specified more than once
        switch (opt) {
            case 'f':
                *filepath = optarg;
                break;

            // print the usage message
            case 'h':
                // basename removes path information
                // POSIX version, can modify the argument.
                print_usage(basename(argv[0]));
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
                // no break
            default:
                GNL_THROW_OPT_EXCEPTION(opt_err, error, "invalid option")
                /* NOT REACHED */
                break;
        }
    }

    return 0;
}

#undef GNL_SHORT_OPTS
#undef GNL_THROW_OPT_EXCEPTION