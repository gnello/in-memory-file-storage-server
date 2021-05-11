#include "./gnl_colorshell.c"
#include "../include/gnl_assert.h"

int gnl_assert(int (*fun)(), const char *fun_desc) {
    int res = fun();

    if (res == 0) {
        gnl_printf_green("PASSED");
    } else {
        gnl_printf_red("FAILED");
    }

    printf(" %s\n", fun_desc);

    return res;
}