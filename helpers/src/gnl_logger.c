#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>
#include <gnl_macro_beg.h>
#include "../include/gnl_logger.h"

/**
 * Return the corresponding level of the given string.
 *
 * @param level The string level.
 *
 * @return      The corresponding level of the given string
 *              on success, -1 otherwise.
 */
static int level_from_string(const char *level) {
    if (strcmp("trace", level) == 0) {
        return GNL_LOGGER_TRACE;
    }

    if (strcmp("debug", level) == 0) {
        return GNL_LOGGER_DEBUG;
    }

    if (strcmp("info", level) == 0) {
        return GNL_LOGGER_INFO;
    }

    if (strcmp("warn", level) == 0) {
        return GNL_LOGGER_WARN;
    }

    if (strcmp("error", level) == 0) {
        return GNL_LOGGER_ERROR;
    }

    errno = EINVAL;

    return -1;
}

/**
 * Return the corresponding string of the given string level.
 *
 * @param level The log level.
 * @param dest  The destination where to write the string level.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
static int level_to_string(const enum gnl_log_level level, char **dest) {
    switch (level) {
        case GNL_LOGGER_TRACE:
            GNL_CALLOC(*dest, 6 * sizeof(char), -1)
            strcpy(*dest, "TRACE");
            break;

        case GNL_LOGGER_DEBUG:
            GNL_CALLOC(*dest, 6 * sizeof(char), -1)
            strcpy(*dest, "DEBUG");
            break;

        case GNL_LOGGER_INFO:
            GNL_CALLOC(*dest, 5 * sizeof(char), -1)
            strcpy(*dest, "INFO");
            break;

        case GNL_LOGGER_WARN:
            GNL_CALLOC(*dest, 5 * sizeof(char), -1)
            strcpy(*dest, "WARN");
            break;

        case GNL_LOGGER_ERROR:
            GNL_CALLOC(*dest, 6 * sizeof(char), -1)
            strcpy(*dest, "ERROR");
            break;
    }

    return 0;
}

struct gnl_logger *gnl_logger_init(const char *path, const char *channel, const char *level) {
    if (path == NULL) {
        errno = EINVAL;

        return NULL;
    }

    struct gnl_logger *logger = (struct gnl_logger *)malloc(sizeof(struct gnl_logger));
    GNL_NULL_CHECK(logger, ENOMEM, NULL)

    // level
    if (level == NULL) {
        logger->level = 0;
    } else {
        logger->level = level_from_string(level);
        GNL_MINUS1_CHECK(logger->level, EINVAL, NULL);
    }

    // path
    GNL_CALLOC(logger->path, strlen(path) + 1, NULL);
    strcpy(logger->path, path);

    // channel
    if (channel == NULL) {
        GNL_CALLOC(logger->channel, 1, NULL);
        strcpy(logger->channel, "\0");
    } else {
        GNL_CALLOC(logger->channel, strlen(channel) + 1, NULL);
        strcpy(logger->channel, channel);
    }

    return logger;
}

void gnl_logger_destroy(struct gnl_logger *logger) {
    free(logger->path);
    free(logger->channel);
    free(logger);
}

/**
 * Check whether a message should be reported or not,
 * based on the current log level.
 *
 * @param logger    The logger instance.
 * @param level     The log level.
 *
 * @return          Returns a positive number if the message
 *                  should be reported, 0 otherwise.
 */
static int should_report(const struct gnl_logger *logger, const enum gnl_log_level level) {
    return logger->level >= level;
}

/**
 * Add the channel to the beginning of the given message.
 *
 * @param logger    The logger instance.
 * @param message   The message where to put the channel.
 * @param dest      The destination where to write the final message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int add_channel_to_message(const struct gnl_logger *logger, const char *message, char **dest) {
    int maxsize = strlen(logger->channel) + 1 + strlen(message) + 1;

    GNL_CALLOC(*dest, maxsize, -1)

    snprintf(*dest, maxsize, "%s.%s", logger->channel, message);

    return 0;
}

/**
 * Add the log level to the beginning of the given message.
 *
 * @param message   The message where to put the channel.
 * @param level     The level of the message.
 * @param dest      The destination where to write the final message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int add_level_to_message(const char *message, const enum gnl_log_level level, char **dest) {
    int res;
    char *string_level;
    int maxsize;

    res = level_to_string(level, &string_level);
    GNL_MINUS1_CHECK(res, errno, -1)

    maxsize = strlen(string_level) + 1 + strlen(message) + 1;

    GNL_CALLOC(*dest, maxsize, -1)

    snprintf(*dest, maxsize, "%s %s", string_level, message);

    free(string_level);

    return 0;
}

/**
 * Add a timestamp to the beginning of the given message.
 *
 * @param message   The message where to put the timestamp.
 * @param dest      The destination where to write the final message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int add_timestamp_to_message(const char *message, char **dest) {
    int maxsize;
    char *timestamp;

    // get timestamp
    GNL_CALLOC(timestamp, 22 * sizeof(char), -1)

    time_t local_time;
    local_time = time(NULL);

    struct tm *tm;
    tm = localtime(&local_time);

    sprintf(timestamp,"%04d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    // add timestamp
    maxsize = strlen(timestamp) + 1 + strlen(message) + 1;
    GNL_CALLOC(*dest, maxsize, -1)

    snprintf(*dest, maxsize, "%s %s", timestamp, message);

    free(timestamp);

    return 0;
}

/**
 * Add a new line character to the given message.
 *
 * @param message   The message where to put the new line character.
 * @param dest      The destination where to write the final message.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int add_new_line_to_message(const char *message, char **dest) {
    int maxsize = strlen(message) + 2;
    GNL_CALLOC(*dest, maxsize, -1)

    snprintf(*dest, maxsize, "%s\n", message);

    return 0;
}

/**
 * Build the message.
 *
 * @param logger    The logger instance.
 * @param message   The raw message to build.
 * @param level     The log level of the message.
 * @param dest     The destination where to write the message.
 *
 * @return         Returns 0 on success, -1 otherwise.
 */
static int build_message(const struct gnl_logger *logger, const char *message, const enum gnl_log_level level, char **dest) {
    int res;
    char *level_message;
    char *scope_message;
    char *timestamp_message;

    res = add_level_to_message(message, level, &level_message);
    GNL_MINUS1_CHECK(res, errno, -1)

    res = add_channel_to_message(logger, level_message, &scope_message);
    GNL_MINUS1_CHECK(res, errno, -1)

    res = add_timestamp_to_message(scope_message, &timestamp_message);
    GNL_MINUS1_CHECK(res, errno, -1)

    res = add_new_line_to_message(timestamp_message, dest);
    GNL_MINUS1_CHECK(res, errno, -1)

    free(level_message);
    free(scope_message);
    free(timestamp_message);

    return 0;
}

/**
 * Report a message into the log file.
 *
 * @param logger    The logger instance.
 * @param message   The message to report.
 * @param level     The log level of the message.
 * @param a_list    The list of arguments.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
static int report(const struct gnl_logger *logger, const char *message, const enum gnl_log_level level, va_list a_list) {
    int res;
    char *dest;
    FILE *log_file;

    res = build_message(logger, message, level, &dest);
    GNL_MINUS1_CHECK(res, errno, -1)

    log_file = fopen(logger->path, "a");
    GNL_NULL_CHECK(log_file, errno, -1);

    // print with variable args
    vfprintf(log_file, dest, a_list);
    fclose(log_file);

    free(dest);

    return 0;
}

int gnl_logger_trace(const struct gnl_logger *logger, const char *message, ...) {
    int res = 0;

    if (should_report(logger, GNL_LOGGER_TRACE)) {
        // get variable args
        va_list a_list;
        va_start(a_list, message);

        res = report(logger, message, GNL_LOGGER_TRACE, a_list);

        va_end(a_list);
    }

    return res;
}

int gnl_logger_debug(const struct gnl_logger *logger, const char *message, ...) {
    int res = 0;

    if (should_report(logger, GNL_LOGGER_DEBUG)) {
        // get variable args
        va_list a_list;
        va_start(a_list, message);

        res = report(logger, message, GNL_LOGGER_DEBUG, a_list);

        va_end(a_list);
    }

    return res;
}

int gnl_logger_info(const struct gnl_logger *logger, const char *message, ...) {
    int res = 0;

    if (should_report(logger, GNL_LOGGER_INFO)) {
        // get variable args
        va_list a_list;
        va_start(a_list, message);

        res = report(logger, message, GNL_LOGGER_INFO, a_list);

        va_end(a_list);
    }

    return res;
}

int gnl_logger_warn(const struct gnl_logger *logger, const char *message, ...) {
    int res = 0;

    if (should_report(logger, GNL_LOGGER_WARN)) {
        // get variable args
        va_list a_list;
        va_start(a_list, message);

        res = report(logger, message, GNL_LOGGER_WARN, a_list);

        va_end(a_list);
    }

    return res;
}

int gnl_logger_error(const struct gnl_logger *logger, const char *message, ...) {
    int res = 0;

    if (should_report(logger, GNL_LOGGER_ERROR)) {
        // get variable args
        va_list a_list;
        va_start(a_list, message);

        res = report(logger, message, GNL_LOGGER_ERROR, a_list);

        va_end(a_list);
    }

    return res;
}

#include <gnl_macro_end.h>
