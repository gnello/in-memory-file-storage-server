
#ifndef GNL_FSS_CONFIG_H
#define GNL_FSS_CONFIG_H

/**
 * Holds the configuration values.
 *
 * thread_workers       Number of workers to span for the server (master-worker
 *                      pattern implementation).
 * capacity             Capacity of the File Storage Server in MB.
 * limit                Maximum number of files stored by the File Storage Server.
 * replacement_policy   Storage replacement policy. Supported policies: 0-FIFO, 1-LRU, 2-LFU.
 * socket               Absolute path of the socket file.
 * log_filepath         Absolute path of the log file.
 * log_level            The log level. Accepted values: trace, debug, info, warn, error.
 */
struct gnl_fss_config {
    int thread_workers;
    int capacity;
    int limit;
    int replacement_policy;
    char *socket;
    char *log_filepath;
    char *log_level;
};

/**
 * Instantiate a gnl_fss_config structure with default values.
 *
 * @return  Returns the gnl_fss_config structure.
 */
struct gnl_fss_config *gnl_fss_config_init();

/**
 * Instantiate a gnl_fss_config structure with env values.
 *
 * @return  Returns the gnl_fss_config structure.
 */
struct gnl_fss_config *gnl_fss_config_init_from_env();

/**
 * Destroy the entire configuration.
 *
 * @param config    The configuration to be destroyed.
 */
void gnl_fss_config_destroy(struct gnl_fss_config *config);

#endif //GNL_FSS_CONFIG_H