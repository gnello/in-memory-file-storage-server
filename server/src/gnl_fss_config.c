#include <stdlib.h>
#include <errno.h>
#include <gnl_storage.h>
#include "../include/gnl_fss_config.h"
#include <gnl_macro_beg.h>

#define GNL_MINUS1_CHECK_FREE_ON_ERROR(ptr, value, error_code, return_value) {    \
    if ((value) == -1) {                                              \
        errno = error_code;                                         \
        free(ptr);                                                  \
                                                                    \
        return return_value;                                        \
    }                                                               \
}

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
    int value = (int)strtol(env_value, &ptr, 10);

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

struct gnl_fss_config *gnl_fss_config_init() {
    struct gnl_fss_config *config = (struct gnl_fss_config *)malloc(sizeof(struct gnl_fss_config));
    GNL_NULL_CHECK(config, ENOMEM, NULL);

    config->thread_workers = 10;
    config->capacity = 100;
    config->limit = 100;
    config->replacement_policy = REPOL_FIFO;
    config->socket = "/tmp/gnl_fss.sk";
    config->log_filepath = "/var/log/gnl_fss.log";
    config->log_level = "error";

    return config;
}

struct gnl_fss_config *gnl_fss_config_init_from_env() {
    struct gnl_fss_config *config = (struct gnl_fss_config *)malloc(sizeof(struct gnl_fss_config));
    GNL_NULL_CHECK(config, ENOMEM, NULL);

    config->thread_workers = get_int_value_from_env("THREAD_WORKERS");
    GNL_MINUS1_CHECK_FREE_ON_ERROR(config, config->thread_workers, EINVAL, NULL)

    config->capacity = get_int_value_from_env("CAPACITY");
    GNL_MINUS1_CHECK_FREE_ON_ERROR(config, config->capacity, EINVAL, NULL)

    config->limit = get_int_value_from_env("LIMIT");
    GNL_MINUS1_CHECK_FREE_ON_ERROR(config, config->limit, EINVAL, NULL)

    config->replacement_policy = get_int_value_from_env("REPLACEMENT_POLICY");
    GNL_MINUS1_CHECK_FREE_ON_ERROR(config, config->limit, EINVAL, NULL)

    config->socket = getenv("SOCKET");
    config->log_filepath = getenv("LOG_FILE");
    config->log_level = getenv("LOG_LEVEL");

    return config;
}

void gnl_fss_config_destroy(struct gnl_fss_config *config) {
    if (config != NULL) {
        free(config);
    }
}

#undef GNL_MINUS1_CHECK_FREE_ON_ERROR
#include <gnl_macro_end.h>