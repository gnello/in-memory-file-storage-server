#include <gnl_queue_t.h>
#include <gnl_macro_beg.h>

/**
 * Scan recursively the given dirname and put n files in
 * the queue. If n=0 put all the found files into the queue.
 *
 * @param dirname   The root dirname to scan.
 * @param n         If n=0, put all files into the queue, otherwise
 *                  put into it only n files.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
struct gnl_queue_t *gnl_opt_rts_scan_dir(const char *dirname, int n) {
    struct gnl_queue_t *queue;

    queue = gnl_queue_init();
    GNL_NULL_CHECK(queue, errno, NULL);

    return queue;
}

#include <gnl_macro_end.h>
