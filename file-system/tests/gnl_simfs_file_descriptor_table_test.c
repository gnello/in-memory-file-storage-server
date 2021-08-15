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

int can_not_put_limit() {
    struct gnl_simfs_file_descriptor_table *table = gnl_simfs_file_descriptor_table_init(0);

    if (table == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_inode_init();

    int res = gnl_simfs_file_descriptor_table_put(table, inode);

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

    struct gnl_simfs_inode *inode_1 = gnl_simfs_inode_init();
    struct gnl_simfs_inode *inode_2 = gnl_simfs_inode_init();
    struct gnl_simfs_inode *inode_3 = gnl_simfs_inode_init();

    int res = gnl_simfs_file_descriptor_table_put(table, inode_1);

    if (res == -1) {
        return -1;
    }

    if (table->size != 1) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_put(table, inode_2);

    if (res == -1) {
        return -1;
    }

    if (table->size != 2) {
        return -1;
    }

    res = gnl_simfs_file_descriptor_table_put(table, inode_3);

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

int main() {
    gnl_printf_yellow("> gnl_socket_request test:\n\n");

    gnl_assert(can_init_a_fdt, "can init a file descriptor table.");

    gnl_assert(can_not_put_limit, "can not put an element into a full file descriptor table.");
    gnl_assert(can_put, "can put an element into a file descriptor table.");

    // the gnl_simfs_file_descriptor_table_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}