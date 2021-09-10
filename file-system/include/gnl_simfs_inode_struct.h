#ifndef GNL_SIMFS_INODE_STRUCT_H
#define GNL_SIMFS_INODE_STRUCT_H

#include <pthread.h>
#include <gnl_list_t.h>

/**
 * File's inode for the Simplified In Memory File System (SIMFS).
 */
struct gnl_simfs_inode {

    // the creation time timestamp of the file
    // within the inode, it is set on file
    // creation and not changed subsequently.
    time_t btime;

    // the last modification timestamp of the file
    // within the inode, it is changed by writes
    time_t mtime;

    // the last access timestamp of the file within
    // the inode, it is changed by reads
    time_t atime;

    // the last status change timestamp of the file
    // within the inode, it is changed by writes or
    // sets on the inode information
    time_t ctime;

    // the size in bytes of the file within the inode
    unsigned int size;

    // the name of the file pointed by
    // the direct_ptr attribute
    char *name;

    // the direct pointer to read from the file
    void *direct_ptr;

    // the buffer to write in a file
    void *buffer;

    // the buffer size in bytes
    int buffer_size;

    // the owner id of the lock, it should be a number > 0:
    // if 0 then the inode is unlocked, if > 0 the inode is locked;
    // we do not use native lock implementation here because
    // the lock status must persist between different methods invocations
    unsigned int locked;

    // the condition variable to track if a file can be locked
    pthread_cond_t file_is_lockable;

    // the reference count of the inode
    unsigned int reference_count;

    // the list containing the reference owners of the inode
    struct gnl_list_t *reference_list;

    // the count of pid that want to lock the pointed file
    unsigned int pending_locks;
};

#endif //GNL_SIMFS_INODE_STRUCT_H