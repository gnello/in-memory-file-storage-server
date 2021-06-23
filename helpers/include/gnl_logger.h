
#ifndef GNL_LOGGER_H
#define GNL_LOGGER_H

/**
 * Levels of logging.
 */
enum gnl_log_level {
    GNL_LOGGER_TRACE = 5,
    GNL_LOGGER_DEBUG = 4,
    GNL_LOGGER_INFO = 3,
    GNL_LOGGER_WARN = 2,
    GNL_LOGGER_ERROR = 1
};

/**
 * The logger struct.
 */
struct gnl_logger {

    // the path where to write the log.
    char *path;

    // The scope of the instance
    char *scope;

    // the log level set.
    enum gnl_log_level level;
};

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
 * @param logger    The logger instance.
 * @param message   The message to log, eventually formatted.
 * @param ...       The eventual variable args,
 *                  to use with a formatted message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_logger_trace(struct gnl_logger *logger, char *message, ...);

/**
 * Log debug level messages.
 *
 * @param logger    The logger instance.
 * @param message   The message to log, eventually formatted.
 * @param ...       The eventual variable args,
 *                  to use with a formatted message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_logger_debug(struct gnl_logger *logger, char *message, ...);

/**
 * Log info level messages.
 *
 * @param logger    The logger instance.
 * @param message   The message to log, eventually formatted.
 * @param ...       The eventual variable args,
 *                  to use with a formatted message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_logger_info(struct gnl_logger *logger, char *message, ...);

/**
 * Log warn level messages.
 *
 * @param logger    The logger instance.
 * @param message   The message to log, eventually formatted.
 * @param ...       The eventual variable args,
 *                  to use with a formatted message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_logger_warn(struct gnl_logger *logger, char *message, ...);

/**
 * Log error level messages.
 *
 * @param logger    The logger instance.
 * @param message   The message to log, eventually formatted.
 * @param ...       The eventual variable args,
 *                  to use with a formatted message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_logger_error(struct gnl_logger *logger, char *message, ...);

#endif //GNL_LOGGER_H
