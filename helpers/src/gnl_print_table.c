#include <stdio.h>

#define GNL_PRINTF_H_INITIAL_SPACE "  "
#define GNL_PRINTF_H_TAB "%-28s"

int printInTable(char *col1_string, char *col2_string) {
    return printf(GNL_PRINTF_H_INITIAL_SPACE GNL_PRINTF_H_TAB "%s", col1_string, col2_string);
}

#undef GNL_PRINTF_H_INITIAL_SPACE
#undef GNL_PRINTF_H_TAB