#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_file_to_pointer.h>
#include <gnl_assert.h>
#include "../src/gnl_simfs_file_table.c"

int can_init_a_ft() {
    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();

    if (table == NULL) {
        return -1;
    }

    if (table->table != NULL) {
        return -1;
    }

    if (table->size != 0) {
        return -1;
    }

    if (table->count != 0) {
        return -1;
    }

    gnl_simfs_file_table_destroy(table);

    return 0;
}

int can_create() {
    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();
    if (table == NULL || table->count > 0) {
        return -1;
    }

    if (gnl_simfs_file_table_get(table, "test") != NULL) {
        return -1;
    }

    if (gnl_simfs_file_table_count(table) != 0) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_table_create(table, "test");
    if (inode == NULL) {
        return -1;
    }

    if (gnl_simfs_file_table_get(table, "test") == NULL) {
        return -1;
    }

    if (gnl_simfs_file_table_count(table) != 1) {
        return -1;
    }

    gnl_simfs_file_table_destroy(table);

    return 0;
}

int can_not_create() {
    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();
    if (table == NULL || table->count > 0) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_table_create(table, "test");
    if (inode == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode_fail = gnl_simfs_file_table_create(table, "test");

    if (inode_fail != NULL) {
        return -1;
    }

    if (errno != EEXIST) {
        return -1;
    }



    gnl_simfs_file_table_destroy(table);

    return 0;
}

int can_get() {
    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();
    if (table == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode1 = gnl_simfs_file_table_create(table, "test1");
    if (inode1 == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode2 = gnl_simfs_file_table_create(table, "test2");
    if (inode2 == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode3 = gnl_simfs_file_table_create(table, "test3");
    if (inode3 == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode4 = gnl_simfs_file_table_create(table, "test4");
    if (inode4 == NULL) {
        return -1;
    }

    if (gnl_simfs_file_table_get(table, "test3") == NULL) {
        return -1;
    }

    if (strcmp("test3", inode3->name) != 0) {
        return -1;
    }

    gnl_simfs_file_table_destroy(table);

    return 0;
}

int can_not_get() {
    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();
    if (table == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_table_get(table, "test");

    if (inode != NULL) {
        return -1;
    }

    if (errno != ENOENT) {
        return -1;
    }

    gnl_simfs_file_table_destroy(table);

    return 0;
}

int can_fflush() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();
    if (table == NULL || table->size > 0) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_table_create(table, "test");
    if (inode == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode_copy = gnl_simfs_inode_copy(inode);
    if (inode_copy == NULL) {
        return -1;
    }

    res = gnl_simfs_inode_append_to_file(inode_copy, content, size);
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_file_table_size(table) > 0) {
        return -1;
    }

    res = gnl_simfs_file_table_fflush(table, inode_copy, size);
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_file_table_size(table) <= 0 || gnl_simfs_file_table_size(table) != size) {
        return -1;
    }

    free(content);
    gnl_simfs_inode_copy_destroy(inode_copy);
    gnl_simfs_file_table_destroy(table);

    return 0;
}

int can_not_fflush() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();
    if (table == NULL || table->size > 0) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_table_create(table, "test");
    if (inode == NULL) {
        return -1;
    }

    res = gnl_simfs_inode_append_to_file(inode, content, size);
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_file_table_size(table) > 0) {
        return -1;
    }

    res = gnl_simfs_file_table_fflush(table, inode, size);
    if (res == 0) {
        return -1;
    }

    if (errno != EINVAL) {
        return -1;
    }

    free(content);
    gnl_simfs_file_table_destroy(table);

    return 0;
}

int can_remove() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();
    if (table == NULL || table->count > 0) {
        return -1;
    }

    if (gnl_simfs_file_table_get(table, "test") != NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_table_create(table, "test");
    if (inode == NULL) {
        return -1;
    }

    if (gnl_simfs_file_table_get(table, "test") == NULL) {
        return -1;
    }

    if (gnl_simfs_file_table_size(table) != 0) {
        return -1;
    }

    if (gnl_simfs_file_table_count(table) != 1) {
        return -1;
    }

    struct gnl_simfs_inode *inode_copy = gnl_simfs_inode_copy(inode);
    if (inode_copy == NULL) {
        return -1;
    }

    res = gnl_simfs_inode_append_to_file(inode_copy, content, size);
    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_table_fflush(table, inode_copy, size);
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_file_table_size(table) != size) {
        return -1;
    }

    res = gnl_simfs_file_table_remove(table, "test");
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_file_table_get(table, "test") != NULL) {
        return -1;
    }

    if (gnl_simfs_file_table_size(table) != 0) {
        return -1;
    }

    if (gnl_simfs_file_table_count(table) != 0) {
        return -1;
    }

    free(content);
    gnl_simfs_inode_copy_destroy(inode_copy);
    gnl_simfs_file_table_destroy(table);

    return 0;
}

int can_not_remove() {
    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();
    if (table == NULL) {
        return -1;
    }

    int res = gnl_simfs_file_table_remove(table, "test");
    if (res == 0) {
        return -1;
    }

    if (errno != ENOENT) {
        return -1;
    }

    gnl_simfs_file_table_destroy(table);

    return 0;
}

int can_get_size() {
    int res;
    long size;
    char *content = NULL;

    res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();
    if (table == NULL || table->size > 0) {
        return -1;
    }

    struct gnl_simfs_inode *inode = gnl_simfs_file_table_create(table, "test");
    if (inode == NULL) {
        return -1;
    }

    struct gnl_simfs_inode *inode_copy = gnl_simfs_inode_copy(inode);
    if (inode_copy == NULL) {
        return -1;
    }

    res = gnl_simfs_inode_append_to_file(inode_copy, content, size);
    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_file_table_fflush(table, inode_copy, size);
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_file_table_size(table) != size) {
        return -1;
    }

    free(content);
    gnl_simfs_inode_copy_destroy(inode_copy);
    gnl_simfs_file_table_destroy(table);

    return 0;
}

int can_get_count() {
    struct gnl_simfs_file_table *table = gnl_simfs_file_table_init();
    if (table == NULL || table->count > 0) {
        return -1;
    }

    struct gnl_simfs_inode *inode1 = gnl_simfs_file_table_create(table, "test1");
    if (inode1 == NULL) {
        return -1;
    }

    if (gnl_simfs_file_table_count(table) != 1) {
        return -1;
    }

    struct gnl_simfs_inode *inode2 = gnl_simfs_file_table_create(table, "test2");
    if (inode2 == NULL) {
        return -1;
    }

    if (gnl_simfs_file_table_count(table) != 2) {
        return -1;
    }

    gnl_simfs_file_table_destroy(table);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_simfs_file_table test:\n\n");

    gnl_assert(can_init_a_ft, "can init a file table.");

    gnl_assert(can_create, "can create an entry in a file table.");
    gnl_assert(can_not_create, "can not create an entry with a filename already present in a file table.");

    gnl_assert(can_get, "can get an entry from a file table.");
    gnl_assert(can_not_get, "can not get a non-existing entry from a file table.");

    gnl_assert(can_fflush, "can flush an entry with a buffer entry in a file table.");
    gnl_assert(can_not_fflush, "can not flush an entry with the original entry in a file table.");

    gnl_assert(can_remove, "can remove an entry from a file table.");
    gnl_assert(can_not_remove, "can not remove a non-existing entry from a file table.");

    gnl_assert(can_get_size, "can get the size in bytes of a file table.");
    gnl_assert(can_get_count, "can get the number of files present into a file table.");

    // the gnl_simfs_file_table_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}