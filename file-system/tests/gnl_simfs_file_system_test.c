#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_file_to_pointer.h>
#include "../src/gnl_simfs_file_system.c"

int can_init_a_filesystem() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL, GNL_SIMFS_RP_NONE);

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

int can_open_o_create() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL, GNL_SIMFS_RP_NONE);

    if (fs == NULL) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1, NULL);
    if (fd != 0) {
        return -1;
    }

    fd = gnl_simfs_file_system_open(fs, "/test/file_2", GNL_SIMFS_O_CREATE, 1, NULL);
    if (fd != 1) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_not_open_o_create() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL, GNL_SIMFS_RP_NONE);

    if (fs == NULL) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1, NULL);
    if (fd != 0) {
        return -1;
    }

    fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1, NULL);
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
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 2, NULL, NULL, GNL_SIMFS_RP_NONE);

    if (fs == NULL) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1, NULL);
    if (fd != 0) {
        return -1;
    }

    fd = gnl_simfs_file_system_open(fs, "/test/file_2", GNL_SIMFS_O_CREATE, 1, NULL);
    if (fd != 1) {
        return -1;
    }

    fd = gnl_simfs_file_system_open(fs, "/test/file_3", GNL_SIMFS_O_CREATE, 1, NULL);
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
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 2, NULL, NULL, GNL_SIMFS_RP_NONE);

    if (fs == NULL) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1, NULL);
    if (fd != 0) {
        return -1;
    }

    for (size_t i=1; i<10240; i++) {
        fd = gnl_simfs_file_system_open(fs, "/test/file_1", 0, 1, NULL);
        if (fd != i) {
            return -1;
        }
    }

    fd = gnl_simfs_file_system_open(fs, "/test/file_1", 0, 1, NULL);
    if (fd != -1) {
        return -1;
    }

    if (errno != ENFILE) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_not_open() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL, GNL_SIMFS_RP_NONE);

    if (fs == NULL) {
        return -1;
    }

    int res = gnl_simfs_file_system_open(fs, "/test/file", 0, 1, NULL);
    if (res != -1) {
        return -1;
    }

    if (errno != ENOENT) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_not_open_lock() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL, GNL_SIMFS_RP_NONE);

    if (fs == NULL) {
        return -1;
    }

    int res = gnl_simfs_file_system_open(fs, "/test/file", GNL_SIMFS_O_CREATE | GNL_SIMFS_O_LOCK, 1, NULL);
    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_system_open(fs, "/test/file", 0, 2, NULL);
    if (res != -1) {
        return -1;
    }

    if (errno != EBUSY) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_open() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL, GNL_SIMFS_RP_NONE);

    if (fs == NULL) {
        return -1;
    }

    int res = gnl_simfs_file_system_open(fs, "/test/file", GNL_SIMFS_O_CREATE, 1, NULL);
    if (res == -1) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file", 0, 1, NULL);
    if (fd == -1) {
        return -1;
    }

    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_write() {
    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(500, 100, NULL, NULL, GNL_SIMFS_RP_NONE);

    if (fs == NULL) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file", GNL_SIMFS_O_CREATE, 1, NULL);
    if (fd == -1) {
        return -1;
    }

    long size;
    char *content = NULL;

    int res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_system_write(fs, fd, content, size, 1, NULL);
    if (res == -1) {
        return -1;
    }

    void *buf;
    size_t count;

    res = gnl_simfs_file_system_read(fs, fd, &buf, &count, 1);
    if (res == -1) {
        return -1;
    }

    if (size != count) {
        return -1;
    }

    if (memcmp(content, buf, size) != 0) {
        return -1;
    }

    free(content);
    free(buf);
    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int can_not_write_memory_limit() {
    long size;
    char *content = NULL;

    int res = gnl_file_to_pointer("./very_big_testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_simfs_file_system *fs = gnl_simfs_file_system_init(1, 1, NULL, NULL, GNL_SIMFS_RP_NONE);

    if (fs == NULL) {
        return -1;
    }

    int fd = gnl_simfs_file_system_open(fs, "/test/file_1", GNL_SIMFS_O_CREATE, 1, NULL);
    if (fd != 0) {
        return -1;
    }

    res = gnl_simfs_file_system_write(fs, fd, content, size, 1, NULL);
    if (res != -1) {
        return -1;
    }

    if (errno != E2BIG) {
        return -1;
    }

    free(content);
    gnl_simfs_file_system_destroy(fs);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_simfs_file_system test:\n\n");

    gnl_assert(can_init_a_filesystem, "can init a file system.");

    gnl_assert(can_open_o_create, "can open a new file with GNL_SIMFS_O_CREATE flag.");
    gnl_assert(can_not_open_o_create, "can not open an existing file with GNL_SIMFS_O_CREATE flag.");

    gnl_assert(can_not_open_files_limit, "can not open a new file if the volume is full.");
    gnl_assert(can_not_open_max_files, "can not open a new file if there are too many open files.");
    gnl_assert(can_not_open, "can not open a file that does not exists.");
    gnl_assert(can_not_open_lock, "can not open a file that is locked.");
    gnl_assert(can_open, "can open a file that exists.");

    gnl_assert(can_write, "can write (and read) a file."); // this method tests also the read method

    // the following test is heavy for valgrind
    //gnl_assert(can_not_write_memory_limit, "can not write a file if there are no space left on the volume.");

    // the gnl_simfs_file_system_destroy method is implicitly tested in every assertion

    printf("\n");
}