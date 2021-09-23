#include "../include/gnl_simfs_monitor.h"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
struct gnl_simfs_monitor {

    // the maximum number of files present
    // reached by file system
    int file_peak;

    // the maximum number of bytes reached
    // by file system
    unsigned long long bytes_peak;

    // the overall number of evictions
    // occurred in the file system
    int file_evictions;

    // the number of files present into
    // the file system
    int file_counter;

    // the number of bytes written into
    // the file system
    unsigned long long bytes_counter;
};

/**
 * {@inheritDoc}
 */
struct gnl_simfs_monitor *gnl_simfs_monitor_init() {
    struct gnl_simfs_monitor *monitor = (struct gnl_simfs_monitor *)malloc(sizeof(struct gnl_simfs_monitor));
    GNL_NULL_CHECK(monitor, ENOMEM, NULL)

    monitor->file_peak = 0;
    monitor->bytes_peak = 0;
    monitor->file_evictions = 0;
    monitor->file_counter = 0;
    monitor->bytes_counter = 0;

    return monitor;
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_monitor_destroy(struct gnl_simfs_monitor *monitor) {
    if (monitor == NULL) {
        return;
    }

    free(monitor);
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_monitor_file_added(struct gnl_simfs_monitor *monitor) {
    // validate the parameters
    GNL_NULL_CHECK(monitor, EINVAL, -1)

    monitor->file_counter++;

    if (monitor->file_counter > monitor->file_peak) {
        monitor->file_peak = monitor->file_counter;
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_monitor_file_removed(struct gnl_simfs_monitor *monitor) {
    // validate the parameters
    GNL_NULL_CHECK(monitor, EINVAL, -1)

    if (monitor->file_counter == 0) {
        errno = EPERM;
        return -1;
    }

    monitor->file_counter--;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_monitor_bytes_added(struct gnl_simfs_monitor *monitor, unsigned long long bytes) {
    // validate the parameters
    GNL_NULL_CHECK(monitor, EINVAL, -1)

    monitor->bytes_counter += bytes;

    if (monitor->bytes_counter > monitor->bytes_peak) {
        monitor->bytes_peak = monitor->bytes_counter;
    }

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_monitor_bytes_removed(struct gnl_simfs_monitor *monitor, unsigned long long bytes) {
    // validate the parameters
    GNL_NULL_CHECK(monitor, EINVAL, -1)

    if (monitor->bytes_counter == 0 || (monitor->bytes_counter - bytes) < 0) {
        errno = EPERM;
        return -1;
    }

    monitor->bytes_counter -= bytes;

    return 0;
}

/**
 * {@inheritDoc}
 */
int gnl_simfs_monitor_eviction_started(struct gnl_simfs_monitor *monitor) {
    // validate the parameters
    GNL_NULL_CHECK(monitor, EINVAL, -1)

    monitor->file_evictions++;

    return 0;
}

#include <gnl_macro_end.h>