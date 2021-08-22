#include <stdio.h>
#include <string.h>
#include <time.h>
#include <gnl_colorshell.h>
#include <gnl_file_to_pointer.h>
#include <gnl_assert.h>
#include "../src/gnl_simfs_inode.c"

int can_init_an_inode() {
    time_t start_time = time(NULL);

    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    time_t end_time = time(NULL);

    if (strcmp(inode->name, "test") != 0) {
        return -1;
    }

    if (inode->creation_time < start_time || inode->creation_time > end_time) {
        return -1;
    }

    if (inode->last_modify_time != 0) {
        return -1;
    }

    if (inode->size != 0) {
        return -1;
    }

    if (inode->reference_count != 0) {
        return -1;
    }

    if (inode->locked != 0) {
        return -1;
    }

    if (inode->direct_ptr != NULL) {
        return -1;
    }

    if (inode->active_hippie_pid != 0) {
        return -1;
    }

    if (inode->waiting_locker_pid != 0) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_lock() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    if (gnl_simfs_inode_is_file_locked(inode) > 0) {
        return -1;
    }

    int res = gnl_simfs_inode_file_lock(inode, 1);
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_inode_is_file_locked(inode) != 1) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_not_lock_different_pid() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_file_lock(inode, 1);
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_inode_is_file_locked(inode) != 1) {
        return -1;
    }

    res = gnl_simfs_inode_file_lock(inode, 2);
    if (res != -1) {
        return -1;
    }

    if (errno != EPERM) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_unlock() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_file_lock(inode, 1);
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_inode_is_file_locked(inode) != 1) {
        return -1;
    }

    res = gnl_simfs_inode_file_unlock(inode, 1);
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_inode_is_file_locked(inode) == 1) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_not_unlock_different_pid() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_file_lock(inode, 1);
    if (res == -1) {
        return -1;
    }

    if (gnl_simfs_inode_is_file_locked(inode) != 1) {
        return -1;
    }

    res = gnl_simfs_inode_file_unlock(inode, 2);
    if (res != -1) {
        return -1;
    }

    if (gnl_simfs_inode_is_file_locked(inode) == 0) {
        return -1;
    }

    if (errno != EPERM) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_increase_refs() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    if (inode->reference_count != 0) {
        return -1;
    }

    int res;
    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_increase_refs(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->reference_count != 13) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_decrease_refs() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res;
    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_increase_refs(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->reference_count != 13) {
        return -1;
    }

    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_decrease_refs(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->reference_count != 0) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_not_decrease_refs() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_decrease_refs(inode);
    if (res != -1) {
        return -1;
    }

    if (inode->reference_count != 0) {
        return -1;
    }

    if (errno != EPERM) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_increase_hippie_pid() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    if (inode->active_hippie_pid != 0) {
        return -1;
    }

    int res;
    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_increase_hippie_pid(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->active_hippie_pid != 13) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_decrease_hippie_pid() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res;
    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_increase_hippie_pid(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->active_hippie_pid != 13) {
        return -1;
    }

    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_decrease_hippie_pid(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->active_hippie_pid != 0) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_not_decrease_hippie_pid() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_decrease_hippie_pid(inode);
    if (res != -1) {
        return -1;
    }

    if (inode->active_hippie_pid != 0) {
        return -1;
    }

    if (errno != EPERM) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_check_hippie_pid() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_has_hippie_pid(inode);
    if (res != 0) {
        return -1;
    }

    res = gnl_simfs_inode_increase_hippie_pid(inode);
    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_inode_has_hippie_pid(inode);
    if (res != 1) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_increase_locker_pid() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    if (inode->waiting_locker_pid != 0) {
        return -1;
    }

    int res;
    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_increase_locker_pid(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->waiting_locker_pid != 13) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_decrease_locker_pid() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res;
    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_increase_locker_pid(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->waiting_locker_pid != 13) {
        return -1;
    }

    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_decrease_locker_pid(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->waiting_locker_pid != 0) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_not_decrease_locker_pid() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_decrease_locker_pid(inode);
    if (res != -1) {
        return -1;
    }

    if (inode->waiting_locker_pid != 0) {
        return -1;
    }

    if (errno != EPERM) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_check_locker_pid() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_has_locker_pid(inode);
    if (res != 0) {
        return -1;
    }

    res = gnl_simfs_inode_increase_locker_pid(inode);
    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_inode_has_locker_pid(inode);
    if (res != 1) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_add_to_file() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    long size;
    char *content = NULL;

    int res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    time_t start_time = time(NULL);

    res = gnl_simfs_inode_append_to_file(inode, content, size);
    if (res == -1) {
        return -1;
    }

    time_t end_time = time(NULL);

    if (inode->last_modify_time < start_time || inode->last_modify_time > end_time) {
        return -1;
    }

    if (inode->size != size) {
        return -1;
    }

    free(content);
    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_copy() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    inode->last_modify_time = time(NULL);
    int res = gnl_simfs_inode_append_to_file(inode, "string", 6);
    if (res == -1) {
        return -1;
    }

    struct gnl_simfs_inode *inode_copy = gnl_simfs_inode_copy(inode);

    if (inode_copy->creation_time != inode->creation_time) {
        return -1;
    }

    if (inode_copy->last_modify_time != inode->last_modify_time) {
        return -1;
    }

    if (inode_copy->size != inode->size) {
        return -1;
    }

    if (strcmp(inode_copy->name, inode->name) != 0) {
        return -1;
    }

    if (inode_copy->direct_ptr != inode->direct_ptr) {
        return -1;
    }

    if (inode_copy->locked != inode->locked) {
        return -1;
    }

    if (inode_copy->reference_count != inode->reference_count) {
        return -1;
    }

    if (inode_copy->active_hippie_pid != inode->active_hippie_pid) {
        return -1;
    }

    if (inode_copy->waiting_locker_pid != inode->waiting_locker_pid) {
        return -1;
    }

    gnl_simfs_inode_copy_destroy(inode_copy);
    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_update() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    time_t creation_time = inode->creation_time;

    char *name = malloc((strlen(inode->name) + 1) * sizeof(char));
    if (name == NULL) {
        return -1;
    }
    strcpy(name, inode->name);

    int locked = inode->locked;
    int reference_count = inode->reference_count;
    int active_hippie_pid = inode->active_hippie_pid;
    int waiting_locker_pid = inode->waiting_locker_pid;

    struct gnl_simfs_inode *new_inode = gnl_simfs_inode_copy(inode);
    if (new_inode == NULL) {
        return -1;
    }

    new_inode->locked = 4;
    new_inode->reference_count = 1;
    new_inode->active_hippie_pid = 2;
    new_inode->waiting_locker_pid = 3;

    time_t start_time = time(NULL);

    int res = gnl_simfs_inode_append_to_file(new_inode, "string", 6);
    if (res != 0) {
        return -1;
    }

    time_t end_time = time(NULL);

    res = gnl_simfs_inode_update(inode, new_inode);
    if (res != 0) {
        return -1;
    }

    if (inode->creation_time != creation_time) {
        return -1;
    }

    if (strcmp(inode->name, name) != 0) {
        return -1;
    }

    if (inode->locked != locked) {
        return -1;
    }

    if (inode->reference_count != reference_count) {
        return -1;
    }

    if (inode->active_hippie_pid != active_hippie_pid) {
        return -1;
    }

    if (inode->waiting_locker_pid != waiting_locker_pid) {
        return -1;
    }

    if (inode->size != 6) {
        return -1;
    }

    char buf[7];
    memcpy(buf, inode->direct_ptr, 6);
    buf[6] = '\0';

    if (strcmp(buf, "string") != 0) {
        return -1;
    }

    if (inode->last_modify_time < start_time || inode->last_modify_time > end_time) {
        return -1;
    }

    //free(name);
    gnl_simfs_inode_copy_destroy(new_inode);
    gnl_simfs_inode_destroy(inode);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_simfs_inode test:\n\n");

    gnl_assert(can_init_an_inode, "can init an inode.");

    gnl_assert(can_lock, "can lock a file within an inode.");
    gnl_assert(can_not_lock_different_pid, "can not lock a not-owned file within an inode.");
    gnl_assert(can_unlock, "can unlock file within an inode.");
    gnl_assert(can_not_unlock_different_pid, "can not unlock a not-owned file within an inode.");

    gnl_assert(can_increase_refs, "can increase an inode reference count.");
    gnl_assert(can_decrease_refs, "can decrease an inode reference count.");
    gnl_assert(can_not_decrease_refs, "can not decrease an inode reference count if it was not previously increased.");

    gnl_assert(can_increase_hippie_pid, "can increase an inode hippie pid.");
    gnl_assert(can_decrease_hippie_pid, "can decrease an inode hippie pid.");
    gnl_assert(can_not_decrease_hippie_pid, "can not decrease an inode hippie pid if it was not previously increased.");
    gnl_assert(can_check_hippie_pid, "can check if an inode has hippie pid.");

    gnl_assert(can_increase_locker_pid, "can increase an inode locker pid.");
    gnl_assert(can_decrease_locker_pid, "can decrease an inode locker pid.");
    gnl_assert(can_not_decrease_locker_pid, "can not decrease an inode locker pid if it was not previously increased.");
    gnl_assert(can_check_locker_pid, "can check if an inode has locker pid.");

    gnl_assert(can_add_to_file, "can add bytes to the file within an inode.");

    gnl_assert(can_copy, "can get a copy of an inode.");
    gnl_assert(can_update, "can update an inode.");

    // the gnl_simfs_inode_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}