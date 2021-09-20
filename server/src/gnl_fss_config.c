#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <gnl_simfs_file_system.h>
#include "../include/gnl_fss_config.h"
#include <gnl_logger.h>
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

    // reset the errno value
    errno = 0;

    // get the value
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

/**
 * Get the real replacement policy value from the env.
 *
 * @param replacement_policy    The pointer where to write the replacement policy
 *                              read from the env.
 *
 * @return                      Returns 0 on success, -1 otherwise.
 */
static int get_replacement_policy_from_env(enum gnl_simfs_replacement_policy *replacement_policy) {
    char *rp = getenv("REPLACEMENT_POLICY");

    int res = 0;

    if ((strcmp("NONE", rp) == 0)) {
        *replacement_policy = GNL_SIMFS_RP_NONE;
    }

    else if ((strcmp("FIFO", rp) == 0)) {
        *replacement_policy = GNL_SIMFS_RP_FIFO;
    }

    else if ((strcmp("LIFO", rp) == 0)) {
        *replacement_policy = GNL_SIMFS_RP_LIFO;
    }

    else if ((strcmp("LRU", rp) == 0)) {
        *replacement_policy = GNL_SIMFS_RP_LRU;
    }

    else if ((strcmp("MRU", rp) == 0)) {
        *replacement_policy = GNL_SIMFS_RP_MRU;
    }

    else if ((strcmp("LFU", rp) == 0)) {
        *replacement_policy = GNL_SIMFS_RP_LFU;
    }

    else {
        errno = EINVAL;
        res = -1;
    }

    return res;
}

/**
 * {@inheritDoc}
 */
struct gnl_fss_config *gnl_fss_config_init() {
    struct gnl_fss_config *config = (struct gnl_fss_config *)malloc(sizeof(struct gnl_fss_config));
    GNL_NULL_CHECK(config, ENOMEM, NULL);

    config->thread_workers = 10;
    config->capacity = 100;
    config->limit = 100;
    config->replacement_policy = GNL_SIMFS_RP_NONE;
    config->socket = "/tmp/gnl_fss.sk";
    config->log_filepath = "/var/log/gnl_fss.log";
    config->log_level = "error";

    return config;
}

/**
 * {@inheritDoc}
 */
struct gnl_fss_config *gnl_fss_config_init_from_env() {
    struct gnl_fss_config *config = (struct gnl_fss_config *)malloc(sizeof(struct gnl_fss_config));
    GNL_NULL_CHECK(config, ENOMEM, NULL);

    config->thread_workers = get_int_value_from_env("THREAD_WORKERS");
    GNL_MINUS1_CHECK_FREE_ON_ERROR(config, config->thread_workers, EINVAL, NULL)

    config->capacity = get_int_value_from_env("CAPACITY");
    GNL_MINUS1_CHECK_FREE_ON_ERROR(config, config->capacity, EINVAL, NULL)

    config->limit = get_int_value_from_env("LIMIT");
    GNL_MINUS1_CHECK_FREE_ON_ERROR(config, config->limit, EINVAL, NULL)

    enum gnl_simfs_replacement_policy rp;
    int res = get_replacement_policy_from_env(&rp);
    GNL_MINUS1_CHECK_FREE_ON_ERROR(config, res, errno, NULL)

    config->replacement_policy = rp;

    config->socket = getenv("SOCKET");
    config->log_filepath = getenv("LOG_FILE");
    config->log_level = getenv("LOG_LEVEL");
    
    return config;
}

/**
 * {@inheritDoc}
 */
void gnl_fss_config_destroy(struct gnl_fss_config *config) {
    if (config != NULL) {
        free(config);
    }
}

#undef GNL_MINUS1_CHECK_FREE_ON_ERROR
#include <gnl_macro_end.h>