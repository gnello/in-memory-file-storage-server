#ifndef GNL_SIMFS_MONITOR_H
#define GNL_SIMFS_MONITOR_H

/**
 * The monitor structure.
 */
struct gnl_simfs_monitor;

/**
 * Create a new monitor instance.
 *
 * @return  Returns the new monitor instance created on success,
 *          NULL otherwise.
 */
extern struct gnl_simfs_monitor *gnl_simfs_monitor_init();

/**
 * Destroy the given monitor.
 *
 * @param monitor   The monitor instance to destroy.
 */
extern void gnl_simfs_monitor_destroy(struct gnl_simfs_monitor *monitor);

/**
 * Track a file addition. It should be called whenever
 * a file is added into the file system.
 *
 * @param monitor   The monitor instance where to track.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_monitor_file_added(struct gnl_simfs_monitor *monitor);

/**
 * Track a file removal. It should be called whenever
 * a file is removed from the file system.
 *
 * @param monitor   The monitor instance where to track.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_monitor_file_removed(struct gnl_simfs_monitor *monitor);

/**
 * Track bytes addition. It should be called whenever
 * some bytes is written into a file.
 *
 * @param monitor   The monitor instance where to track.
 * @param bytes     The number of bytes written.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_monitor_bytes_added(struct gnl_simfs_monitor *monitor, unsigned long long bytes);

/**
 * Track a file removal. It should be called whenever
 * some bytes is deleted from a file.
 *
 * @param monitor   The monitor instance where to track.
 * @param bytes     The number of bytes removed.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_monitor_bytes_removed(struct gnl_simfs_monitor *monitor, unsigned long long bytes);

/**
 * Track evictions. It should be called whenever
 * an eviction is started by the file system.
 *
 * @param monitor   The monitor instance where to track.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_simfs_monitor_eviction_started(struct gnl_simfs_monitor *monitor);

#endif //GNL_SIMFS_MONITOR_H