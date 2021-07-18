
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_logger.c"

#define BUFFER_SIZE 1000
#define LOGGER_TEST_FILE "./logger.txt"

int test_logger_level(int should_be_present, char *log_level, char *logged_level, int (*fun)(const struct gnl_logger *logger, const char *message, ...)) {
    struct gnl_logger *logger;

    logger = gnl_logger_init(LOGGER_TEST_FILE, "gnl_logger_test", log_level);

    if (logger == NULL) {
        return -1;
    }

    char *expected = "test message";
    if (fun(logger, expected) != 0) {
        return -1;
    }

    gnl_logger_destroy(logger);

    char actual[BUFFER_SIZE];
    FILE *tmp = fopen(LOGGER_TEST_FILE, "r");

    if (tmp == NULL) {
        if (should_be_present) {
            return -1;
        }

        return 0;
    }

    fgets(actual, BUFFER_SIZE, tmp);
    fclose(tmp);
    remove(LOGGER_TEST_FILE);

    if (should_be_present && strstr(actual, expected) == NULL) {
        return -1;
    } else if (!should_be_present && strstr(actual, expected) != NULL) {
        return -1;
    }

    if (should_be_present && strstr(actual, logged_level) == NULL) {
        return -1;
    } else if (!should_be_present && strstr(actual, logged_level) != NULL){
        return -1;
    }

    return 0;
}

int can_create_a_logger() {
    struct gnl_logger *logger;

    logger = gnl_logger_init(LOGGER_TEST_FILE, "gnl_logger_test", "debug");

    if (logger == NULL) {
        return -1;
    }

    gnl_logger_destroy(logger);

    return 0;
}

int can_not_create_a_logger_without_path() {
    struct gnl_logger *logger;

    logger = gnl_logger_init(NULL, "gnl_logger_test", "debug");

    if (logger != NULL) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    return 0;
}

int can_create_a_logger_without_channel() {
    struct gnl_logger *logger;

    logger = gnl_logger_init(LOGGER_TEST_FILE, NULL, "debug");

    if (logger == NULL) {
        return -1;
    }

    gnl_logger_destroy(logger);

    return 0;
}

int can_create_a_logger_without_level() {
    struct gnl_logger *logger;

    logger = gnl_logger_init(LOGGER_TEST_FILE, "gnl_logger_test", NULL);

    if (logger == NULL) {
        return -1;
    }

    gnl_logger_destroy(logger);

    return 0;
}

int can_trace_trace() {
    return test_logger_level(1, "trace", "TRACE", gnl_logger_trace);
}

int can_trace_debug() {
    return test_logger_level(1, "trace", "DEBUG", gnl_logger_debug);
}

int can_trace_info() {
    return test_logger_level(1, "trace", "INFO", gnl_logger_info);
}

int can_trace_warn() {
    return test_logger_level(1, "trace", "WARN", gnl_logger_warn);
}

int can_trace_error() {
    return test_logger_level(1, "trace", "ERROR", gnl_logger_error);
}

int can_not_debug_trace() {
    return test_logger_level(0, "debug", "TRACE", gnl_logger_trace);
}

int can_debug_debug() {
    return test_logger_level(1, "debug", "DEBUG", gnl_logger_debug);
}

int can_debug_info() {
    return test_logger_level(1, "debug", "INFO", gnl_logger_info);
}

int can_debug_warn() {
    return test_logger_level(1, "debug", "WARN", gnl_logger_warn);
}

int can_debug_error() {
    return test_logger_level(1, "debug", "ERROR", gnl_logger_error);
}

int can_not_info_trace() {
    return test_logger_level(0, "info", "TRACE", gnl_logger_trace);
}

int can_not_info_debug() {
    return test_logger_level(0, "info", "DEBUG", gnl_logger_debug);
}

int can_info_info() {
    return test_logger_level(1, "info", "INFO", gnl_logger_info);
}

int can_info_warn() {
    return test_logger_level(1, "info", "WARN", gnl_logger_warn);
}

int can_info_error() {
    return test_logger_level(1, "info", "ERROR", gnl_logger_error);
}

int can_not_warn_trace() {
    return test_logger_level(0, "warn", "TRACE", gnl_logger_trace);
}

int can_not_warn_debug() {
    return test_logger_level(0, "warn", "DEBUG", gnl_logger_debug);
}

int can_not_warn_info() {
    return test_logger_level(0, "warn", "INFO", gnl_logger_info);
}

int can_warn_warn() {
    return test_logger_level(1, "warn", "WARN", gnl_logger_warn);
}

int can_warn_error() {
    return test_logger_level(1, "warn", "ERROR", gnl_logger_error);
}

int can_not_error_trace() {
    return test_logger_level(0, "error", "TRACE", gnl_logger_trace);
}

int can_not_error_debug() {
    return test_logger_level(0, "error", "DEBUG", gnl_logger_debug);
}

int can_not_error_info() {
    return test_logger_level(0, "error", "INFO", gnl_logger_info);
}

int can_not_error_warn() {
    return test_logger_level(0, "error", "WARN", gnl_logger_warn);
}

int can_error_error() {
    return test_logger_level(1, "error", "ERROR", gnl_logger_error);
}

int main() {
    gnl_printf_yellow("> gnl_logger test:\n\n");

    gnl_assert(can_create_a_logger, "can create a logger.");
    gnl_assert(can_not_create_a_logger_without_path, "can not create a logger without a valid path.");
    gnl_assert(can_create_a_logger_without_channel, "can create a logger without a channel.");
    gnl_assert(can_create_a_logger_without_level, "can create a logger without a log level.");

    gnl_assert(can_trace_trace, "can report a trace message with log level \"trace\".");
    gnl_assert(can_trace_debug, "can report a debug message with log level \"trace\".");
    gnl_assert(can_trace_info, "can report a info message with log level \"trace\".");
    gnl_assert(can_trace_warn, "can report a warn message with log level \"trace\".");
    gnl_assert(can_trace_error, "can report a error message with log level \"trace\".");

    gnl_assert(can_not_debug_trace, "can not report a trace message with log level \"debug\".");
    gnl_assert(can_debug_debug, "can report a debug message with log level \"debug\".");
    gnl_assert(can_debug_info, "can report a info message with log level \"debug\".");
    gnl_assert(can_debug_warn, "can report a warn message with log level \"debug\".");
    gnl_assert(can_debug_error, "can report a error message with log level \"debug\".");

    gnl_assert(can_not_info_trace, "can not report a trace message with log level \"info\".");
    gnl_assert(can_not_info_debug, "can not report a debug message with log level \"info\".");
    gnl_assert(can_info_info, "can report a info message with log level \"info\".");
    gnl_assert(can_info_warn, "can report a warn message with log level \"info\".");
    gnl_assert(can_info_error, "can report a error message with log level \"info\".");

    gnl_assert(can_not_warn_trace, "can not report a trace message with log level \"warn\".");
    gnl_assert(can_not_warn_debug, "can not report a debug message with log level \"warn\".");
    gnl_assert(can_not_warn_info, "can bot report a info message with log level \"warn\".");
    gnl_assert(can_warn_warn, "can report a warn message with log level \"warn\".");
    gnl_assert(can_warn_error, "can report a error message with log level \"warn\".");

    gnl_assert(can_not_error_trace, "can not report a trace message with log level \"error\".");
    gnl_assert(can_not_error_debug, "can not report a debug message with log level \"error\".");
    gnl_assert(can_not_error_info, "can bot report a info message with log level \"error\".");
    gnl_assert(can_not_error_warn, "can report a warn message with log level \"error\".");
    gnl_assert(can_error_error, "can report a error message with log level \"error\".");

    // the gnl_logger_destroy method is implicitly tested in every assert.

    printf("\n");
}

#undef BUFFER_SIZE
#undef LOGGER_TEST_FILE