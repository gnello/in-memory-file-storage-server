#ifndef GNL_SIMFS_EVICTED_FILE_H
#define GNL_SIMFS_EVICTED_FILE_H

/**
 * The evicted file structure. This structure is
 * returned as an element of the evicted list set during
 * an open or write operations.
 */
struct gnl_simfs_evicted_file {

    // the name of the file evicted
    char *name;

    // the buffer of bytes evicted
    void *bytes;

    // the number of bytes evicted
    size_t count;
};

/**
 * Create a new evicted_file instance.
 *
 * @return  Returns the new gnl_simfs_evicted_file created on success,
 *          NULL otherwise.
 */
extern struct gnl_simfs_evicted_file *gnl_simfs_evicted_file_init();

/**
 * Destroy the given evicted_file instance.
 *
 * @param evicted_file   The evicted_file instance to destroy.
 */
extern void gnl_simfs_evicted_file_destroy(struct gnl_simfs_evicted_file *evicted_file);

#endif //GNL_SIMFS_EVICTED_FILE_H