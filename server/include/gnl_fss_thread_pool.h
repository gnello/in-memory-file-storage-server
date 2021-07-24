
#ifndef GNL_FSS_THREAD_POOL_H
#define GNL_FSS_THREAD_POOL_H

#include <gnl_ts_bb_queue_t.h>
#include <gnl_logger.h>

/**
 * Holds the thread pool information.
 */
struct gnl_fss_thread_pool;

/**
 * Create a new thread pool.
 *
 * @param size          The number of workers to put in the thread pool.
 * @param storage       The storage instance to use to store the files.
 * @param config        The configuration instance of the server.
 *
 * @return              Returns a gnl_fss_thread_pool struct on success,
 *                      NULL otherwise.
 */
extern struct gnl_fss_thread_pool *gnl_fss_thread_pool_init(int size, struct gnl_storage *storage,
        const struct gnl_fss_config *config);

/**
 * Destroy the thread pool.
 *
 * @param thread_pool   The thread pool to be destroyed.
 */
extern void gnl_fss_thread_pool_destroy(struct gnl_fss_thread_pool *thread_pool);

/**
 * Dispatch the given message to any thread present
 * into the given thread pool.
 *
 * @param thread_pool   The tread pool were to dispatch the message.
 * @param message       The message to be dispatched.
 *
 * @return              Return 0 on success, -1 otherwise.
 */
extern int gnl_fss_thread_pool_dispatch(struct gnl_fss_thread_pool *thread_pool, void *message);

/**
 * Return the channel file descriptor where to read a ready client file descriptor.
 * This channel is used by the workers to warn the master that a
 * client request is handled.
 *
 * @param thread_pool   The tread pool were to get the channel.
 *
 * @return              The file descriptor of the master channel on success,
 *                      -1 otherwise.
 */
extern int gnl_fss_thread_pool_master_channel(struct gnl_fss_thread_pool *thread_pool);

#endif //GNL_FSS_THREAD_POOL_H