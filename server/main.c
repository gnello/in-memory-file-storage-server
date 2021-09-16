#include <stdio.h>
#include <gnl_txtenv.h>
#include "./src/gnl_fss_opt_handler.c"
#include "./src/gnl_fss_config.c"
#include "./src/gnl_fss_server.c"

int main(int argc, char * argv[]) {
    char opt_err = '\0';
    char *error = "";
    char *filename = NULL;
    struct gnl_fss_config *config;

    // parse the options
    if (gnl_fss_opt_handler_parse(argc, argv, &filename, &opt_err, &error) != 0) {
        // if there is a custom error message
        if (opt_err != '\0') {
            printf("%s: %s -- '%c'\n", argv[0], error, opt_err);
        } else {
            // print errno message
            perror("Error while parsing the options");
        }

        return -1;
    }

    // if a filename is read on the stdin (-f opt)
    if (filename != NULL) {
        // load configuration into the env
        if (gnl_txtenv_load(filename, 0) != 0) {
            perror("Error on load configuration from file");

            return -1;
        }

        // load configuration from the env
        config = gnl_fss_config_init_from_env();
        if (config == NULL) {
            perror("Error on load server configuration from env");

            return -1;
        }
    } else {
        // no filename read, warn the user...
        printf("Warning: no configuration file provided, using default configuration...\n");

        // load the default configuration
        config = gnl_fss_config_init();
        if (config == NULL) {
            perror("Error on load server default configuration");

            return -1;
        }
    }

    // run the server
    int res = 0;
    if (gnl_fss_server_start(config) != 0) {
        perror("Error from the server");

        res = -1;
    }

    gnl_fss_config_destroy(config);

    return res;
}