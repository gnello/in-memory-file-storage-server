#include <stdio.h>
#include "../includes/gnl_colorshell.h"

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

int gnl_printf_red(char * message) {
    return printf(RED "%s" RESET, message);
}

int gnl_printf_green(char * message) {
    return printf(GRN "%s" RESET, message);
}

int gnl_printf_yellow(char * message) {
    return printf(YEL "%s" RESET, message);
}

int gnl_printf_blu(char * message) {
    return printf(BLU "%s" RESET, message);
}

int gnl_printf_magenta(char * message) {
    return printf(MAG "%s" RESET, message);
}

int gnl_printf_cyan(char * message) {
    return printf(CYN "%s" RESET, message);
}

int gnl_printf_white(char * message) {
    return printf(WHT "%s" RESET, message);
}
