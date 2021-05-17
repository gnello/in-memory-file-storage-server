#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include <errno.h>
#include "../include/gnl_fss_opt_handler.h"

#define GNL_SHORT_OPTS ":hf:"
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

/**
 * Print the two arguments in two columns.
 *
 * @param col1_string   The firs column value.
 * @param col2_string   The second column value.
 *
 * @return int          Returns always 0.
 */
static int print_in_table(char *col1_string, char *col2_string) {
    printf(GNL_PRINTF_H_INITIAL_SPACE GNL_PRINTF_H_TAB "%s", col1_string, col2_string);

    return 0;
}

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

    print_in_table("-h", "Print this message and exit.\n");
    print_in_table("-f FILENAME", "Start the server with the FILENAME configuration file.\n");
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
#undef GNL_PRINTF_H_INITIAL_SPACE
#undef GNL_PRINTF_H_TAB
#undef GNL_THROW_OPT_EXCEPTION