
#ifndef GNL_FSS_THREAD_POOL_H
#define GNL_FSS_THREAD_POOL_H

#include <gnl_ts_bb_queue_t.h>

/**
 * Holds the thread pool information.
 */
struct gnl_fss_thread_pool;

/**
 * Create a new thread pool.
 *
 * @param size          The number of workers to put in the thread pool.
 *
 * @return              Returns a gnl_fss_thread_pool struct on success,
 *                      NULL otherwise.
 */
extern struct gnl_fss_thread_pool *gnl_fss_thread_pool_init(int size);

/**
 * Destroy the thread pool.
 *
 * @param thread_pool   The thread pool to be destroyed.
 */
extern void gnl_fss_thread_pool_destroy(struct gnl_fss_thread_pool *thread_pool);

#endif //GNL_FSS_THREAD_POOL_H