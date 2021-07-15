
#include <pthread.h>
#include "../include/gnl_fss_worker.h"
#include <gnl_macro_beg.h>

pthread_t *gnl_fss_thread_pool_init(int size, struct gnl_fss_worker_config *worker_config)
{
    // allocate memory for the thread pool
    pthread_t *thread_pool = malloc(size * sizeof(pthread_t));
    GNL_NULL_CHECK(thread_pool, ENOMEM, NULL)

    int args = 1;

    // instantiate the thread pool
    for (size_t i=0; i<size; i++) {
        pthread_create(&thread_pool[i], NULL, &gnl_fss_worker_handle, (void *)worker_config);
    }

    return thread_pool;
}

void gnl_fss_thread_pool_destroy(pthread_t *thread_pool) {
    free(thread_pool);
}

#include <gnl_macro_end.h>