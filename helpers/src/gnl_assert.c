#include "./gnl_colorshell.c"

void gnl_assert(int (*fun)(), char *fun_desc) {
    if (fun() == 0) {
        gnl_printf_green("PASSED");
    } else {
        gnl_printf_red("FAILED");
    }

    printf(" %s\n", fun_desc);
}