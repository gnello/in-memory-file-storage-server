#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_simfs_inode.c"
#include "../src/gnl_simfs_file_descriptor_table.c"

int can_init_a_fdt() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    if (table->size != 0) {
        return -1;
    }

    if (table->limit != 100) {
        return -1;
    }

    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_not_put() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(0);

    if (table == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    int res = gnl_simfs_file_descriptor_table_put(table, inode, 1);

    if (res != -1) {
        return -1;
    }

    if (errno != EMFILE) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);
    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_put() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    if (table->size != 0) {
        return -1;
    }

    struct gnl_simfs_inode *inode_1 = gnl_simfs_inode_init("test1");
    struct gnl_simfs_inode *inode_2 = gnl_simfs_inode_init("test2");
    struct gnl_simfs_inode *inode_3 = gnl_simfs_inode_init("test3");

    int res = gnl_simfs_file_descriptor_table_put(table, inode_1, 1);

    if (res == -1) {
        return -1;
    }

    if (table->size != 1) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_put(table, inode_2, 1);

    if (res == -1) {
        return -1;
    }

    if (table->size != 2) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_put(table, inode_3, 1);

    if (res == -1) {
        return -1;
    }

    if (table->size != 3) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode_1);
    gnl_simfs_inode_destroy(inode_2);
    gnl_simfs_inode_destroy(inode_3);

    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_not_get_empty() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_descriptor_table_get(table, 0, 1);

    if (inode != NULL) {
        return -1;
    }

    if (errno != EBADF) {
        return -1;
    }

    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_not_get_not_existing() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode_1 = gnl_simfs_inode_init("test1");
    struct gnl_simfs_inode *inode_2 = gnl_simfs_inode_init("test2");
    struct gnl_simfs_inode *inode_3 = gnl_simfs_inode_init("test3");

    int res = gnl_simfs_file_descriptor_table_put(table, inode_1, 1);

    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_put(table, inode_2, 1);

    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_put(table, inode_3, 1);

    if (res == -1) {
        return -1;
    }

    if (table->size != 3) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_descriptor_table_get(table, 4, 1);

    if (inode != NULL) {
        return -1;
    }

    if (errno != EBADF) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode_1);
    gnl_simfs_inode_destroy(inode_2);
    gnl_simfs_inode_destroy(inode_3);
    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_not_get_removed() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode_1 = gnl_simfs_inode_init("test1");
    struct gnl_simfs_inode *inode_2 = gnl_simfs_inode_init("test2");
    struct gnl_simfs_inode *inode_3 = gnl_simfs_inode_init("test3");

    int res = gnl_simfs_file_descriptor_table_put(table, inode_1, 1);

    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_put(table, inode_2, 1);

    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_put(table, inode_3, 1);

    if (res == -1) {
        return -1;
    }

    if (table->size != 3) {
        return -1;
    }

    struct gnl_simfs_inode *inode_ok = gnl_simfs_file_descriptor_table_get(table, 0, 1);

    if (inode_ok == NULL) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_remove(table, 0, 1);

    if (res != 0) {
        return -1;
    }

    if (table->size != 2) {
        return -1;
    }

    struct gnl_simfs_inode *inode_null = gnl_simfs_file_descriptor_table_get(table, 0, 1);

    if (inode_null != NULL) {
        return -1;
    }

    if (errno != EBADF) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode_1);
    gnl_simfs_inode_destroy(inode_2);
    gnl_simfs_inode_destroy(inode_3);
    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_not_get_perm() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode_1 = gnl_simfs_inode_init("test");

    int res = gnl_simfs_file_descriptor_table_put(table, inode_1, 1);

    if (res == -1) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_descriptor_table_get(table, 0, 2);

    if (inode != NULL) {
        return -1;
    }

    if (errno != EPERM) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode_1);
    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_not_remove() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    int res = gnl_simfs_file_descriptor_table_remove(table, 4, 1);

    if (res != -1) {
        return -1;
    }

    if (errno != EBADF) {
        return -1;
    }

    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_not_remove_perm() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode_1 = gnl_simfs_inode_init("test");

    int res = gnl_simfs_file_descriptor_table_put(table, inode_1, 1);

    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_remove(table, 0, 2);

    if (res != -1) {
        return -1;
    }

    if (errno != EPERM) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode_1);
    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_get() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode_1 = gnl_simfs_inode_init("test1");
    struct gnl_simfs_inode *inode_2 = gnl_simfs_inode_init("test2");
    struct gnl_simfs_inode *inode_3 = gnl_simfs_inode_init("test3");

    int res = gnl_simfs_file_descriptor_table_put(table, inode_1, 1);

    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_put(table, inode_2, 1);

    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_put(table, inode_3, 1);

    if (res == -1) {
        return -1;
    }

    if (table->size != 3) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_descriptor_table_get(table, 0, 1);

    if (inode == NULL) {
        return -1;
    }

    inode = gnl_simfs_file_descriptor_table_get(table, 1, 1);

    if (inode == NULL) {
        return -1;
    }

    inode = gnl_simfs_file_descriptor_table_get(table, 2, 1);

    if (inode == NULL) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode_1);
    gnl_simfs_inode_destroy(inode_2);
    gnl_simfs_inode_destroy(inode_3);
    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_remove() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    if (table->size != 0) {
        return -1;
    }

    struct gnl_simfs_inode *inode_1 = gnl_simfs_inode_init("test1");
    struct gnl_simfs_inode *inode_2 = gnl_simfs_inode_init("test2");
    struct gnl_simfs_inode *inode_3 = gnl_simfs_inode_init("test3");

    int fd_1 = gnl_simfs_file_descriptor_table_put(table, inode_1, 1);

    if (fd_1 == -1) {
        return -1;
    }

    if (table->size != 1) {
        return -1;
    }

    int fd_2 = gnl_simfs_file_descriptor_table_put(table, inode_2, 1);

    if (fd_2 == -1) {
        return -1;
    }

    if (table->size != 2) {
        return -1;
    }

    int fd_3 = gnl_simfs_file_descriptor_table_put(table, inode_3, 1);

    if (fd_3 == -1) {
        return -1;
    }

    if (table->size != 3) {
        return -1;
    }

    int res = gnl_simfs_file_descriptor_table_remove(table, fd_2, 1);

    if (res != 0) {
        return -1;
    }

    if (table->size != 2) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode_1);
    gnl_simfs_inode_destroy(inode_2);
    gnl_simfs_inode_destroy(inode_3);

    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_put_after_remove() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    if (table->size != 0) {
        return -1;
    }

    struct gnl_simfs_inode *inode_1 = gnl_simfs_inode_init("test1");
    struct gnl_simfs_inode *inode_2 = gnl_simfs_inode_init("test2");
    struct gnl_simfs_inode *inode_3 = gnl_simfs_inode_init("test3");
    struct gnl_simfs_inode *inode_4 = gnl_simfs_inode_init("test4");

    int fd_1 = gnl_simfs_file_descriptor_table_put(table, inode_1, 1);

    if (fd_1 == -1) {
        return -1;
    }

    if (table->size != 1) {
        return -1;
    }

    int fd_2 = gnl_simfs_file_descriptor_table_put(table, inode_2, 1);

    if (fd_2 == -1) {
        return -1;
    }

    if (table->size != 2) {
        return -1;
    }

    int fd_3 = gnl_simfs_file_descriptor_table_put(table, inode_3, 1);

    if (fd_3 == -1) {
        return -1;
    }

    if (table->size != 3) {
        return -1;
    }

    int res = gnl_simfs_file_descriptor_table_remove(table, fd_2, 1);

    if (res != 0) {
        return -1;
    }

    if (table->size != 2) {
        return -1;
    }

    int fd_4 = gnl_simfs_file_descriptor_table_put(table, inode_4, 1);

    if (fd_4 == -1) {
        return -1;
    }

    if (table->size != 3) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode_1);
    gnl_simfs_inode_destroy(inode_2);
    gnl_simfs_inode_destroy(inode_3);
    gnl_simfs_inode_destroy(inode_4);

    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int can_remove_all() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(100);

    if (table == NULL) {
        return -1;
    }

    if (table->size != 0) {
        return -1;
    }

    struct gnl_simfs_inode *inode_1 = gnl_simfs_inode_init("test1");
    struct gnl_simfs_inode *inode_2 = gnl_simfs_inode_init("test2");
    struct gnl_simfs_inode *inode_3 = gnl_simfs_inode_init("test3");

    int fd_1 = gnl_simfs_file_descriptor_table_put(table, inode_1, 1);

    if (fd_1 == -1) {
        return -1;
    }

    if (table->size != 1) {
        return -1;
    }

    int fd_2 = gnl_simfs_file_descriptor_table_put(table, inode_2, 1);

    if (fd_2 == -1) {
        return -1;
    }

    if (table->size != 2) {
        return -1;
    }

    int fd_3 = gnl_simfs_file_descriptor_table_put(table, inode_3, 1);

    if (fd_3 == -1) {
        return -1;
    }

    if (table->size != 3) {
        return -1;
    }

    int res = gnl_simfs_file_descriptor_table_remove(table, fd_2, 1);

    if (res != 0) {
        return -1;
    }

    if (table->size != 2) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_remove(table, fd_1, 1);

    if (res != 0) {
        return -1;
    }

    if (table->size != 1) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_remove(table, fd_3, 1);

    if (res != 0) {
        return -1;
    }

    if (table->size != 0) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode_1);
    gnl_simfs_inode_destroy(inode_2);
    gnl_simfs_inode_destroy(inode_3);

    gnl_simfs_file_descriptor_table_destroy(table);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_simfs_file_descriptor_table test:\n\n");

    gnl_assert(can_init_a_fdt, "can init a file descriptor table.");

    gnl_assert(can_not_put, "can not put an element into a full file descriptor table.");
    gnl_assert(can_put, "can put an element into a file descriptor table.");

    gnl_assert(can_not_get_empty, "can not get an element from an empty file descriptor table.");
    gnl_assert(can_not_get_not_existing, "can not get a non-existing element from a file descriptor table.");
    gnl_assert(can_not_get_removed, "can not get a removed element from a file descriptor table.");
    gnl_assert(can_not_get_perm, "can not get a non-owned element from a file descriptor table.");
    gnl_assert(can_get, "can get an element from a file descriptor table.");

    gnl_assert(can_not_remove, "can not remove an element from an empty file descriptor table.");
    gnl_assert(can_not_remove_perm, "can not remove a not-owned element from a file descriptor table.");
    gnl_assert(can_remove, "can remove an element from a file descriptor table.");

    gnl_assert(can_put_after_remove, "can put an element into a file descriptor table after a remove.");
    gnl_assert(can_remove_all, "can remove all the elements from a file descriptor table.");

    // the gnl_simfs_file_descriptor_table_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}