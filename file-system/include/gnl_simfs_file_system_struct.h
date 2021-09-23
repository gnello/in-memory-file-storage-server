#ifndef GNL_SIMFS_FILE_SYSTEM_STRUCT_H
#define GNL_SIMFS_FILE_SYSTEM_STRUCT_H

/**
 * The file system structure.
 */
struct gnl_simfs_file_system {

    // the file system file table, contains all the inodes of the
    // files present into the file system
    struct gnl_simfs_file_table *file_table;

    // the number of files that can be handled by the file system
    int files_limit;

    // the memory allocable in bytes by the file system
    unsigned long long memory_limit;

    // contains all the open files in a precisely time,
    // the index is the file descriptor, the value is a
    // copy of the inode of the file.
    struct gnl_simfs_file_descriptor_table *file_descriptor_table;

    // the lock of the file system
    pthread_mutex_t mtx;

    // the logger instance to use for logging
    struct gnl_logger *logger;

    // the replacement policy to adopt in case the file system reaches
    // the given memory_limit or the given file_limit
    enum gnl_simfs_replacement_policy replacement_policy;

    // the monitor instance to store the operations stats
    struct gnl_simfs_monitor *monitor;
};

#endif //GNL_SIMFS_FILE_SYSTEM_STRUCT_H