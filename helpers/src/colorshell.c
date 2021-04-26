#include <stdio.h>

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

/**
 * Print the message in red.
 *
 * @param message   The message to print.
 *
 * @return          The standard printf return.
 */
int gnl_printf_red(char * message) {
    return printf(RED "%s" RESET, message);
}

/**
 * Print the message in green.
 *
 * @param message   The message to print.
 *
 * @return          The standard printf return.
 */
int gnl_printf_green(char * message) {
    return printf(GRN "%s" RESET, message);
}

/**
 * Print the message in yellow.
 *
 * @param message   The message to print.
 *
 * @return          The standard printf return.
 */
int gnl_printf_yellow(char * message) {
    return printf(YEL "%s" RESET, message);
}

/**
 * Print the message in blu.
 *
 * @param message   The message to print.
 *
 * @return          The standard printf return.
 */
int gnl_printf_blu(char * message) {
    return printf(BLU "%s" RESET, message);
}

/**
 * Print the message in magenta.
 *
 * @param message   The message to print.
 *
 * @return          The standard printf return.
 */
int gnl_printf_magenta(char * message) {
    return printf(MAG "%s" RESET, message);
}

/**
 * Print the message in cyan.
 *
 * @param message   The message to print.
 *
 * @return          The standard printf return.
 */
int gnl_printf_cyan(char * message) {
    return printf(CYN "%s" RESET, message);
}

/**
 * Print the message in white.
 *
 * @param message   The message to print.
 *
 * @return          The standard printf return.
 */
int gnl_printf_white(char * message) {
    return printf(WHT "%s" RESET, message);
}
