#include <stdio.h>
#include <string.h>
#include "./src/gnl_opt_handler.c"

int main(int argc, char * argv[]) {
    struct gnl_opt_handler *handler;
    handler = gnl_opt_handler_init();

    if (handler == NULL) {
        perror("Error on opt_handler initialization");
        exit(EXIT_FAILURE);
    }

    char opt_err = 0;
    char *error = NULL;

    // parse the options
    if (gnl_opt_handler_parse_opt(handler, argc, argv, &opt_err, &error) != 0) {
        if (opt_err > 0) {
            printf("Error while parsing the option '-%c': %s\n", opt_err, error);
        } else {
            perror("Error while parsing the options");
        }

        return -1;
    }

    // handle the parsed options
    if (gnl_opt_handler_handle(handler) != 0) {
        perror("Error while processing a command");
    }

    gnl_opt_handler_destroy(handler);

    return 0;
}