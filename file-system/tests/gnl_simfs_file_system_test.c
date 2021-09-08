#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_simfs_file_system.c"

int can_init_a_filesystem() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL);

    if (fs == NULL) {
        return -1;
    }

    // 1048576 is the size of 500 MB in bytes
    if (fs->memory_limit != (500 * 1048576)) {
        return -1;
    }

    if (fs->files_limit != 100) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_open_file_o_create() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL);

    if (fs == NULL) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1);
    if (fd != 0) {
        return -1;
    }

    fd = gnl_simfs_file_system_open(fs, "/test/file_2", GNL_SIMFS_O_CREATE, 1);
    if (fd != 1) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_not_open_file_o_create() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL);

    if (fs == NULL) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1);
    if (fd != 0) {
        return -1;
    }

    fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1);
    if (fd != -1) {
        return -1;
    }

    if (errno != EEXIST) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_not_open_files_limit() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 2, NULL, NULL);

    if (fs == NULL) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1);
    if (fd != 0) {
        return -1;
    }

    fd = gnl_simfs_file_system_open(fs, "/test/file_2", GNL_SIMFS_O_CREATE, 1);
    if (fd != 1) {
        return -1;
    }

    fd = gnl_simfs_file_system_open(fs, "/test/file_3", GNL_SIMFS_O_CREATE, 1);
    if (fd != -1) {
        return -1;
    }

    if (errno != EDQUOT) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_not_open_max_files() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 2, NULL, NULL);

    if (fs == NULL) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1);
    if (fd != 0) {
        return -1;
    }

    for (size_t i=1; i<10240; i++) {
        fd = gnl_simfs_file_system_open(fs, "/test/file_1", 0, 1);
        if (fd != i) {
            return -1;
        }
    }

    fd = gnl_simfs_file_system_open(fs, "/test/file_1", 0, 1);
    if (fd != -1) {
        return -1;
    }

    if (errno != ENFILE) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_not_open_file() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL);

    if (fs == NULL) {
        return -1;
    }

    int res = gnl_simfs_file_system_open(fs, "/test/file", 0, 1);
    if (res != -1) {
        return -1;
    }

    if (errno != ENOENT) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_simfs_file_system_test test:\n\n");

    gnl_assert(can_init_a_filesystem, "can init a file system.");

    gnl_assert(can_open_file_o_create, "can open a new file with GNL_SIMFS_O_CREATE flag.");
    gnl_assert(can_not_open_file_o_create, "can not open an existing file with GNL_SIMFS_O_CREATE flag.");

    gnl_assert(can_not_open_files_limit, "can not open a new file if the volume is full.");
    gnl_assert(can_not_open_max_files, "can not open a new file if there are too many open files.");
    gnl_assert(can_not_open_file, "can not open a file that does not exists.");
    //gnl_assert(can_open_file, "can open a file that exists.");

    // the gnl_simfs_file_system_destroy method is implicitly tested in every assertion

    printf("\n");
}