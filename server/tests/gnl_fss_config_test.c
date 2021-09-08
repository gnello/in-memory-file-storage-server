#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_txtenv.h>
#include "../src/gnl_fss_config.c"

int can_load_default() {
    struct gnl_fss_config *config = gnl_fss_config_init();
    if (config == NULL) {
        return -1;
    }

    if (config->thread_workers != 10) {
        return -1;
    }

    if (config->capacity != 100) {
        return -1;
    }

    if (config->limit != 100) {
        return -1;
    }

    //TODO: sistemare
//    if (config->replacement_policy != REPOL_FIFO) {
//        return -1;
//    }

    if (strcmp(config->socket, "/tmp/gnl_fss.sk") != 0) {
        return -1;
    }

    if (strcmp(config->log_filepath, "/var/log/gnl_fss.log") != 0) {
        return -1;
    }

    if (strcmp(config->log_level, "error") != 0) {
        return -1;
    }

    gnl_fss_config_destroy(config);

    return 0;
}

int can_load_env() {
    if (gnl_txtenv_load("./test_valid_config.txt", 0) != 0) {
        return -1;
    }

    struct gnl_fss_config *config = gnl_fss_config_init_from_env();
    if (config == NULL) {
        return -1;
    }

    if (config->thread_workers != 2) {
        return -1;
    }

    if (config->capacity != 23) {
        return -1;
    }

    if (config->limit != 45) {
        return -1;
    }

    //TODO: sistemare
//    if (config->replacement_policy != REPOL_LFU) {
//        return -1;
//    }

    if (strcmp(config->socket, "/tmp/fss_test.sk") != 0) {
        return -1;
    }

    if (strcmp(config->log_filepath, "/var/log/fss_test.log") != 0) {
        return -1;
    }

    if (strcmp(config->log_level, "debug") != 0) {
        return -1;
    }

    unsetenv("THREAD_WORKERS");
    unsetenv("CAPACITY");
    unsetenv("LIMIT");
    unsetenv("REPLACEMENT_POLICY");
    unsetenv("SOCKET");
    unsetenv("LOG_FILE");
    unsetenv("LOG_LEVEL");

    gnl_fss_config_destroy(config);

    return 0;
}

int can_not_load_with_error() {
    struct gnl_fss_config *config = gnl_fss_config_init_from_env();
    if (config != NULL) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}



int main() {
    gnl_printf_yellow("> gnl_fss_config test:\n\n");

    gnl_assert(can_load_default, "can load a default configuration.");
    gnl_assert(can_load_env, "can load the configuration from the env.");
    gnl_assert(can_not_load_with_error, "can not load an incorrect configuration from the env.");

    // the gnl_fss_config_destroy method is implicitly tested in every assertion

    printf("\n");
}