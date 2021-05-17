#include <stdio.h>
#include <gnl_txtenv.h>
#include <gnl_fss_opt_handler.h>
#include <gnl_fss_config.h>
#include <gnl_fss_storage.h>

int main(int argc, char * argv[]) {
    char opt_err = '\0';
    char *error = "";
    char *filename = NULL;
    gnl_fss_config *config;

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

    // if a filename is read on the stdin
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
        // no filename read, alert the user...
        printf("Warning: no configuration file provided, using default configuration...\n");

        // load the default configuration
        config = gnl_fss_config_init();
        if (config == NULL) {
            perror("Error on load server default configuration");

            return -1;
        }
    }

    printf("capacity: %d\n", config->capacity);

    // instance the filesystem
    //gnl_in_memory_filesystem_init(config->capacity, config->limit, config->replacement_policy);


    return 0;
}