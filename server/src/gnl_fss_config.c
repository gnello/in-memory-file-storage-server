#include <stdlib.h>
#include <errno.h>
#include <gnl_fss_storage.h>
#include "../include/gnl_fss_config.h"

#define GNL_NULL_CHECK(ptr, error_code, return_value) { \
    if (ptr == NULL) {                                  \
        errno = error_code;                             \
                                                        \
        return return_value;                            \
    }                                                   \
}

#define GNL_MINUS1_CHECK(ptr, error_code, return_value) {   \
    if (ptr == -1) {                                        \
        errno = error_code;                                 \
                                                            \
        return return_value;                                \
    }                                                       \
}

struct gnl_fss_config {
    int thread_workers;
    int capacity;
    int limit;
    int replacement_policy;
    char *socket;
    char *logfile;
};

/**
 * Convert the env char *value to int value.
 *
 * @param name  The env property name.
 *
 * @return      Returns the int value of an env property.
 */
static int get_int_value_from_env(const char *name) {
    char *env_value = getenv(name);

    if (env_value == NULL) {
        errno = EINVAL;

        return -1;
    }

    char *ptr = NULL;
    int value = strtol(env_value, &ptr, 10);

    // if no digits found
    if (env_value == ptr) {
        errno = EINVAL;

        return -1;
    }

    // if there was an error
    if (errno != 0) {
        return -1;
    }

    return value;
}

/**
 * Set the config with default values.
 *
 * @param config    The configuration structure.
 */
struct gnl_fss_config *gnl_fss_config_init() {
    struct gnl_fss_config *config = (struct gnl_fss_config *)malloc(sizeof(struct gnl_fss_config));
    GNL_NULL_CHECK(config, ENOMEM, NULL);

    config->thread_workers = 10;
    config->capacity = 100;
    config->limit = 100;
    config->replacement_policy = REPOL_FIFO;
    config->socket = "/tmp/gnl_fss.sk";
    config->logfile = "/var/log/gnl_fss.log";

    return config;
}

struct gnl_fss_config *gnl_fss_config_init_from_env() {
    struct gnl_fss_config *config = (struct gnl_fss_config *)malloc(sizeof(struct gnl_fss_config));
    GNL_NULL_CHECK(config, ENOMEM, NULL);

    config->thread_workers = get_int_value_from_env("THREAD_WORKERS");
    GNL_MINUS1_CHECK(config->thread_workers, EINVAL, NULL)

    config->capacity = get_int_value_from_env("CAPACITY");
    GNL_MINUS1_CHECK(config->capacity, EINVAL, NULL)

    config->limit = get_int_value_from_env("LIMIT");
    GNL_MINUS1_CHECK(config->limit, EINVAL, NULL)

    config->replacement_policy = get_int_value_from_env("REPLACEMENT_POLICY");
    GNL_MINUS1_CHECK(config->limit, EINVAL, NULL)

    config->socket = getenv("SOCKET");
    config->logfile = getenv("LOG_FILE");

    return config;
}

#undef GNL_NULL_CHECK
#undef GNL_MINUS1_CHECK