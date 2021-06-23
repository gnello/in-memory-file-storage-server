
#ifndef GNL_LOGGER_H
#define GNL_LOGGER_H

struct gnl_logger;

/**
 * Create a new logger.
 *
 * @param path  The path where to create the file log.
 * @param scope The scope of the logger instance.
 * @param level The level of the logger instance.
 *              Accepted values: trace, debug, info, warn, error.
 *
 * @return      The new logger instance on success,
 *              NULL otherwise.
 */
extern struct gnl_logger *gnl_logger_init(char *path, char *scope, char *level);

/**
 * Destroy the given logger.
 *
 * @param logger    The logger to destroy.
 */
extern void gnl_logger_destroy(struct gnl_logger *logger);

/**
 * Log trace level messages.
 *
 * @param message   The message to log.
 * @param logger    The logger instance.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_logger_trace(struct gnl_logger *logger, char *message);

/**
 * Log debug level messages.
 *
 * @param message   The message to log.
 * @param logger    The logger instance.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_logger_debug(struct gnl_logger *logger, char *message);

/**
 * Log info level messages.
 *
 * @param message   The message to log.
 * @param logger    The logger instance.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_logger_info(struct gnl_logger *logger, char *message);

/**
 * Log warn level messages.
 *
 * @param message   The message to log.
 * @param logger    The logger instance.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_logger_warn(struct gnl_logger *logger, char *message);

/**
 * Log error level messages.
 *
 * @param message   The message to log.
 * @param logger    The logger instance.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_logger_error(struct gnl_logger *logger, char *message);

#endif //GNL_LOGGER_H
