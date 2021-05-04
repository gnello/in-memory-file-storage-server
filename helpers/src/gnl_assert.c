#include "./gnl_colorshell.c"
#include "../includes/gnl_assert.h"

void gnl_assert(int (*fun)(), const char *fun_desc) {
    if (fun() == 0) {
        gnl_printf_green("PASSED");
    } else {
        gnl_printf_red("FAILED");
    }

    printf(" %s\n", fun_desc);
}