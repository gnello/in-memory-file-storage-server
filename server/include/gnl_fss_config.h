
#ifndef GNL_FSS_CONFIG_H
#define GNL_FSS_CONFIG_H

/**
 * Holds the configuration values.
 *
 * struct gnl_fss_config {
 *      int thread_workers;
 *      int capacity;
 *      int limit;
 *      int replacement_policy;
 *      char *socket;
 *      char *logfile;
 * };
 *
 * thread_workers       Number of workers to span for the server (master-worker
 *                      pattern implementation).
 * capacity             Capacity of the File Storage Server in MB.
 * limit                Maximum number of files stored by the File Storage Server.
 * replacement_policy   Storage replacement policy. Supported policies: 0-FIFO, 1-LRU, 2-LFU.
 * socket               Absolute path of the socket file.
 * logfile              Absolute path of the log file.
 */
typedef struct gnl_fss_config gnl_fss_config;

/**
 * Instantiate a gnl_fss_config structure with default values.
 *
 * @return  Returns the gnl_fss_config structure.
 */
gnl_fss_config *gnl_fss_config_init();

/**
 * Instantiate a gnl_fss_config structure with env values.
 *
 * @return  Returns the gnl_fss_config structure.
 */
gnl_fss_config *gnl_fss_config_init_from_env();

/**
 * Destroy the entire configuration.
 *
 * @param config    The configuration to be destroyed.
 */
void gnl_fss_config_destroy(gnl_fss_config *config);

#endif //GNL_FSS_CONFIG_H