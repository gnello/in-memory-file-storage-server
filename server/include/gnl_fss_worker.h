
#ifndef GNL_FSS_WORKER_H
#define GNL_FSS_WORKER_H

#include <gnl_ts_bb_queue_t.h>
#include <gnl_logger.h>

#define GNL_FSS_WORKER_TERMINATE (-1970)

/**
 * Holds the worker configuration.
 */
struct gnl_fss_worker;

/**
 * Create a new worker config.
 *
 * @param id            The id of the worker.
 * @param worker_queue  The queue where to receive a ready file descriptor
 *                      from a main thread.
 * @param pipe_channel  The pipe channel where to send the result to a
 *                      main thread.
 * @param storage       The storage instance to use to store the files.
 * @param config        The configuration instance of the server.
 *
 * @return gnl_queue_t  Returns the new worker config created on success,
 *                      NULL otherwise.
 */
extern struct gnl_fss_worker *gnl_fss_worker_init(pthread_t id, struct gnl_ts_bb_queue_t *worker_queue,
        int pipe_channel, struct gnl_storage *storage, const struct gnl_fss_config *config);

/**
 * Destroy a worker config. Attention: this method only deletes the
 * reference to the worker config, it does not delete his internal
 * information given at the moment of the initialization. You have
 * to destroy it manually.
 *
 * @param worker The worker to be destroyed.
 */
extern void gnl_fss_worker_destroy(struct gnl_fss_worker *worker);

/**
 * Handle a client request dequeueing a ready file descriptor from the
 * worker queue given within the worker configuration passed using the
 * args argument, then return the result through the pipe channel given
 * within the worker configuration.
 *
 * @param args  It must contains the worker configuration struct.
 *
 * @return      This method never returns on success, otherwise it
 *              returns -1.
 */
extern void *gnl_fss_worker_handle(void* args);

#endif //GNL_FSS_WORKER_H