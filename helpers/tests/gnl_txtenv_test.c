#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_txtenv.c"

int can_load_no_overwrite() {
    // pre test
    if (getenv("ENV") != NULL) {
        return -1;
    }

    if (getenv("TEST1") != NULL) {
        return -1;
    }

    if (getenv("TEST2") != NULL) {
        return -1;
    }

    if (getenv("TEST3") != NULL) {
        return -1;
    }

    // exec command
    if((gnl_txtenv_load("./test.txt", 0)) != 0) {
        return -1;
    }

    // test
    if (strcmp("test", getenv("ENV")) != 0) {
        return -1;
    }

    if (strcmp("test1", getenv("TEST1")) != 0) {
        return -1;
    }

    if (strcmp("test2", getenv("TEST2")) != 0) {
        return -1;
    }

    if (strcmp("test3", getenv("TEST3")) != 0) {
        return -1;
    }

    return 0;
}

int can_load_overwrite() {
    if((gnl_txtenv_load("./test.txt", 1)) != 0) {
        return -1;
    }

    if (strcmp("override", getenv("TEST3")) != 0) {
        return -1;
    }

    return 0;
}

int cannot_load_empty() {
    if((gnl_txtenv_load("./test.txt", 1)) != 0) {
        return -1;
    }

    if (getenv("TEST4") != NULL) {
        return -1;
    }

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_testenv test:\n\n");

    gnl_assert(can_load_no_overwrite, "can load a config file into the env without overwrite.");
    gnl_assert(can_load_overwrite, "can load a config file into the env with overwrite.");
    gnl_assert(cannot_load_empty, "cannot load an empty value into the env.");

    printf("\n");
}