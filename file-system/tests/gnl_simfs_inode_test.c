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

    if (inode->btime < start_time || inode->btime > end_time) {
        return -1;
    }

    if (inode->ctime < start_time || inode->ctime > end_time) {
        return -1;
    }

    if (inode->mtime != 0) {
        return -1;
    }

    if (inode->atime != 0) {
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

    if (inode->pending_locks != 0) {
        return -1;
    }

    if (inode->buffer != NULL) {
        return -1;
    }

    if (inode->buffer_size != 0) {
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

    if (errno != EACCES) {
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

    if (errno != EACCES) {
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

int can_check_refs() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_has_refs(inode);
    if (res != 0) {
        return -1;
    }

    res = gnl_simfs_inode_increase_refs(inode);
    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_inode_has_refs(inode);
    if (res != 1) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_increase_pending_locks() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    if (inode->pending_locks != 0) {
        return -1;
    }

    int res;
    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_increase_pending_locks(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->pending_locks != 13) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_decrease_pending_locks() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res;
    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_increase_pending_locks(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->pending_locks != 13) {
        return -1;
    }

    for (size_t i=0; i<13; i++) {
        res = gnl_simfs_inode_decrease_pending_locks(inode);
        if (res == -1) {
            return -1;
        }
    }

    if (inode->pending_locks != 0) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_not_decrease_pending_locks() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_decrease_pending_locks(inode);
    if (res != -1) {
        return -1;
    }

    if (inode->pending_locks != 0) {
        return -1;
    }

    if (errno != EPERM) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_check_pending_locks() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    if (inode == NULL) {
        return -1;
    }

    int res = gnl_simfs_inode_has_pending_locks(inode);
    if (res != 0) {
        return -1;
    }

    res = gnl_simfs_inode_increase_pending_locks(inode);
    if (res == -1) {
        return -1;
    }

    res = gnl_simfs_inode_has_pending_locks(inode);
    if (res != 1) {
        return -1;
    }

    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_write() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    long size;
    char *content = NULL;

    int res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    time_t start_time = time(NULL);

    res = gnl_simfs_inode_write(inode, content, size);
    if (res == -1) {
        return -1;
    }

    time_t end_time = time(NULL);

    if (inode->ctime < start_time || inode->ctime > end_time) {
        return -1;
    }

    if (inode->buffer == NULL) {
        return -1;
    }

    if (inode->buffer_size != size) {
        return -1;
    }

    free(content);
    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_copy() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    inode->mtime = time(NULL);
    int res = gnl_simfs_inode_write(inode, "string", 6);
    if (res == -1) {
        return -1;
    }

    struct gnl_simfs_inode *inode_copy = gnl_simfs_inode_copy(inode);

    if (inode_copy->btime != inode->btime) {
        return -1;
    }

    if (inode_copy->mtime != inode->mtime) {
        return -1;
    }

    if (inode_copy->atime != inode->atime) {
        return -1;
    }

    if (inode_copy->ctime < inode->ctime) {
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

    if (inode_copy->pending_locks != inode->pending_locks) {
        return -1;
    }

    if (inode_copy->buffer != NULL) {
        return -1;
    }

    if (inode_copy->buffer_size != 0) {
        return -1;
    }

    gnl_simfs_inode_copy_destroy(inode_copy);
    gnl_simfs_inode_destroy(inode);

    return 0;
}

int can_fflush() {
    struct gnl_simfs_inode *inode = gnl_simfs_inode_init("test");

    time_t now = time(NULL);

    int res = gnl_simfs_inode_write(inode, "string", 6);
    if (res <= 0) {
        return -1;
    }

    if (inode->direct_ptr != NULL) {
        return -1;
    }

    if (inode->size != 0) {
        return -1;
    }

    res = gnl_simfs_inode_fflush(inode);
    if (res != 0) {
        return -1;
    }

    if (inode->mtime < now) {
        return -1;
    }

    if (inode->ctime < now) {
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

    res = gnl_simfs_inode_write(inode, "anotherstring", 13);
    if (res <= 0) {
        return -1;
    }

    res = gnl_simfs_inode_write(inode, "thefinalstring", 14);
    if (res <= 0) {
        return -1;
    }

    if (inode->size != 6) {
        return -1;
    }

    res = gnl_simfs_inode_fflush(inode);
    if (res != 0) {
        return -1;
    }

    if (inode->size != 33) {
        return -1;
    }

    char buf2[34];
    memcpy(buf2, inode->direct_ptr, 33);
    buf2[33] = '\0';

    if (strcmp(buf2, "stringanotherstringthefinalstring") != 0) {
        return -1;
    }

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
    gnl_assert(can_check_refs, "can check if an inode has references to it.");

    gnl_assert(can_increase_pending_locks, "can increase an inode pending locks.");
    gnl_assert(can_decrease_pending_locks, "can decrease an inode pending locks.");
    gnl_assert(can_not_decrease_pending_locks, "can not decrease an inode pending locks if it was not previously increased.");
    gnl_assert(can_check_pending_locks, "can check if an inode has pending locks.");

    gnl_assert(can_write, "can write bytes into the file within an inode.");

    gnl_assert(can_copy, "can get a copy of an inode.");
    gnl_assert(can_fflush, "can fflush an inode.");

    // the gnl_simfs_inode_destroy method is implicitly tested in every
    // assert, if you don't believe it, run this tests with
    // valgrind and look for memory leaks, good luck!

    printf("\n");
}