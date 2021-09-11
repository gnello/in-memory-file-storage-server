#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "../include/gnl_fss_waiting_list.h"
#include <gnl_list_t.h>
#include <gnl_queue_t.h>
#include <gnl_macro_beg.h>

/**
 * Macro to acquire the lock.
 */
#define GNL_FSS_LOCK_ACQUIRE(return_value) {                                \
    int lock_acquire_res = pthread_mutex_lock(&(waiting_list->mtx));        \
    GNL_MINUS1_CHECK(lock_acquire_res, errno, return_value)                 \
}

/**
 * Macro to release the lock.
 */
#define GNL_FSS_LOCK_RELEASE(return_value) {                                \
    int lock_release_res = pthread_mutex_unlock(&(waiting_list->mtx));      \
    GNL_MINUS1_CHECK(lock_release_res, errno, return_value)                 \
}

/**
 * Macro to compare two values, if they are not equals
 * return an error and release the lock.
 */
#define GNL_FSS_COMPARE(actual, expected_error, errno_code, return_value) {         \
    if ((actual) == (expected_error)) {                                             \
        errno = errno_code;                                                         \
        GNL_FSS_LOCK_RELEASE(return_value)                                          \
                                                                                    \
        return return_value;                                                        \
    }                                                                               \
}

/**
 * Macro to check that a return value is not NULL,
 * on error release the lock.
 */
#define GNL_FSS_NULL_CHECK(x, error_code, return_code) {        \
    GNL_FSS_COMPARE(x, NULL, error_code, return_code)           \
}

/**
 * Macro to check that a return value is not -1,
 * on error release the lock.
 */
#define GNL_FSS_MINUS1_CHECK(x, error_code, return_code) {      \
    GNL_FSS_COMPARE(x, -1, error_code, return_code)             \
}

/**
 * {@inheritDoc}
 */
struct gnl_fss_waiting_list {

    // the target list
    struct gnl_list_t *target_list;

    // the presence list, it used to see
    // if a pid was removed from the waiting list
    struct gnl_list_t *presence_list;

    // the lock of the waiting list
    pthread_mutex_t mtx;
};

/**
 * The element struct of the waiting list.
 */
struct gnl_fss_waiting_list_internal_el {

    // the target
    char *target;

    // the thread-safe non-blocking queue, it
    // used to store the waiting pid
    struct gnl_queue_t *queue;
};

/**
 * Destroy a gnl_fss_waiting_list_el structure.
 *
 * @param ptr   The gnl_fss_waiting_list_el structure to destroy.
 */
static void destroy_el(void *ptr) {
    // destroy the queue
    gnl_socket_request_destroy(((struct gnl_fss_waiting_list_el *)ptr)->request);

    // destroy the element
    free(ptr);
}

/**
 * Destroy a gnl_fss_waiting_list_internal_el structure.
 *
 * @param ptr   The gnl_fss_waiting_list_internal_el structure to destroy.
 */
static void destroy_internal_el(void *ptr) {
    // destroy the target
    free(((struct gnl_fss_waiting_list_internal_el *)ptr)->target);

    // destroy the queue
    gnl_queue_destroy(((struct gnl_fss_waiting_list_internal_el *)ptr)->queue, destroy_el);

    // destroy the element
    free(ptr);
}

/**
 * Destroy a given pointer.
 *
 * @param ptr   The pointer to destroy.
 */
static void destroy_pointer(void *ptr) {
    free(ptr);
}

/**
 * Compare two waiting_list elements.
 *
 * @param a The actual waiting_list element.
 * @param b The expected waiting_list element.
 *
 * @return  Returns 0 if a==b, if a > b returns a value
 *          greater than zero, if a < b returns a value
 *          less than 0.
 */
static int compare_el(const void *a, const void *b) {
    struct gnl_fss_waiting_list_internal_el a_el;
    struct gnl_fss_waiting_list_internal_el b_el;

    a_el = *(struct gnl_fss_waiting_list_internal_el *)a;
    b_el = *(struct gnl_fss_waiting_list_internal_el *)b;

    return strcmp(a_el.target, b_el.target);
}

/**
 * Compare two int elements.
 *
 * @param a The actual int element.
 * @param b The expected int element.
 *
 * @return  Returns 0 if a==b, if a > b returns a value
 *          greater than zero, if a < b returns a value
 *          less than 0.
 */
static int compare_int(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

/**
 * {@inheritDoc}
 */
struct gnl_fss_waiting_list *gnl_fss_waiting_list_init() {
    // create the instance
    struct gnl_fss_waiting_list *waiting_list = (struct gnl_fss_waiting_list *)malloc(sizeof(struct gnl_fss_waiting_list));
    GNL_NULL_CHECK(waiting_list, ENOMEM, NULL)

    // init the lists
    waiting_list->target_list = NULL;
    waiting_list->presence_list = NULL;

    // initialize lock
    int res = pthread_mutex_init(&(waiting_list->mtx), NULL);
    GNL_MINUS1_CHECK(res, errno, NULL)

    return waiting_list;
}

/**
 * {@inheritDoc}
 */
void gnl_fss_waiting_list_destroy(struct gnl_fss_waiting_list *waiting_list) {
    // check the parameters
    if (waiting_list == NULL) {
        return;
    }

    // destroy the presence list
    gnl_list_destroy(&(waiting_list->presence_list), free);

    // destroy the target list
    gnl_list_destroy(&(waiting_list->target_list), destroy_internal_el);

    // destroy the lock, proceed on error
    pthread_mutex_destroy(&(waiting_list->mtx));

    // destroy the waiting list
    free(waiting_list);
}

/**
 * {@inheritDoc}
 */
int gnl_fss_waiting_list_push(struct gnl_fss_waiting_list *waiting_list, const char *target, int pid,
        struct gnl_socket_request *request) {
    // acquire the lock
    GNL_FSS_LOCK_ACQUIRE(-1)

    // check the parameters
    GNL_FSS_NULL_CHECK(waiting_list, EINVAL, -1)
    GNL_FSS_NULL_CHECK(request, EINVAL, -1)

    int res;

    // copy the target
    char *target_copy = calloc((strlen(target) + 1), sizeof(char));
    GNL_FSS_NULL_CHECK(target_copy, ENOMEM, -1)

    strncpy(target_copy, target, strlen(target));

    // create the gnl_fss_waiting_list_el struct
    struct gnl_fss_waiting_list_el *el = (struct gnl_fss_waiting_list_el *)malloc(sizeof(struct gnl_fss_waiting_list_el));
    GNL_FSS_NULL_CHECK(el, ENOMEM, -1)

    // initialize the el
    el->pid = pid;
    el->request = request;

    // create a mock waiting_list element
    struct gnl_fss_waiting_list_internal_el internal_el = { target_copy, 0 };

    // if the target is not in the waiting_list create a new
    // waiting_list element and put it into the target list
    if (gnl_list_search(waiting_list->target_list, &internal_el, compare_el) == 0) {
        // create the waiting_list element
        struct gnl_fss_waiting_list_internal_el *new_internal_el = (struct gnl_fss_waiting_list_internal_el *)malloc(sizeof(struct gnl_fss_waiting_list_internal_el));
        GNL_FSS_NULL_CHECK(new_internal_el, ENOMEM, -1)

        new_internal_el->target = target_copy;

        // init the queue
        new_internal_el->queue = gnl_queue_init();
        GNL_FSS_NULL_CHECK(new_internal_el->queue, errno, -1)

        // put the el into the new waiting_list element
        res = gnl_queue_enqueue(new_internal_el->queue, el);
        GNL_FSS_MINUS1_CHECK(res, errno, -1)

        // put the new waiting_list element in the target list
        res = gnl_list_append(&(waiting_list->target_list), new_internal_el);
        GNL_FSS_MINUS1_CHECK(res, errno, -1)
    }
    // else get the existent target element and put the pid
    // into its queue
    else {
        // get the existent target element
        struct gnl_list_t *current = waiting_list->target_list;

        while (current != NULL) {
            // if target is found
            if (compare_el(current->el, &internal_el) == 0) {
                // add the pid to the target queue
                res = gnl_queue_enqueue(((struct gnl_fss_waiting_list_internal_el *)(current->el))->queue, el);
                GNL_FSS_MINUS1_CHECK(res, errno, -1)

                // stop searching
                break;
            }

            // move to the next element
            current = current->next;
        }

        // free memory
        free(target_copy);
    }

    // copy the pid for the presence list
    unsigned int *pid_copy_presence = malloc(sizeof(unsigned int));
    GNL_FSS_NULL_CHECK(pid_copy_presence, ENOMEM, -1)

    *pid_copy_presence = pid;

    // add the pid to the presence list
    res = gnl_list_append(&(waiting_list->presence_list), (void *)pid_copy_presence);
    GNL_FSS_MINUS1_CHECK(res, errno, -1)

    // release the lock
    GNL_FSS_LOCK_RELEASE(-1)

    return 0;
}

/**
 * {@inheritDoc}
 */
struct gnl_fss_waiting_list_el *gnl_fss_waiting_list_pop(struct gnl_fss_waiting_list *waiting_list, const char *target) {
    // acquire the lock
    GNL_FSS_LOCK_ACQUIRE(NULL)

    // check the parameters
    GNL_FSS_NULL_CHECK(waiting_list, EINVAL, NULL)

    int res;
    struct gnl_fss_waiting_list_el *popped_el;

    // copy the target
    char *target_copy = calloc((strlen(target) + 1), sizeof(char));
    GNL_FSS_NULL_CHECK(target_copy, ENOMEM, NULL)

    strncpy(target_copy, target, strlen(target));

    // create a mock waiting_list element
    struct gnl_fss_waiting_list_internal_el internal_el = { target_copy, 0 };

    // if the target is not in the waiting_list
    if (gnl_list_search(waiting_list->target_list, &internal_el, compare_el) == 0) {
        // this is not an error, simply
        // the target has no waiting pid
        errno = 0;

        // free memory
        free(target_copy);

        // release the lock
        GNL_FSS_LOCK_RELEASE(NULL)

        return NULL;
    }

    // get the existent target element
    struct gnl_list_t *current = waiting_list->target_list;

    while (current != NULL) {
        // if target is found
        if (compare_el(current->el, &internal_el) == 0) {

            // iterate until we find a valid waiting pid or until the target queue is empty
            int valid_pid_found = 0;

            while (valid_pid_found == 0) {
                // reset the errno
                errno = 0;

                // get the pid from the target queue
                void *popped_el_raw = gnl_queue_dequeue(((struct gnl_fss_waiting_list_internal_el *) (current->el))->queue);

                if (popped_el_raw == NULL) {
                    // free memory
                    free(target_copy);

                    GNL_FSS_LOCK_RELEASE(NULL)

                    // let the errno bubble

                    return NULL;
                }

                // implicit cast the void * to struct gnl_fss_waiting_list_el *
                popped_el = popped_el_raw;

                // check if the pid is still present
                if (gnl_list_search(waiting_list->presence_list, &(popped_el->pid), compare_int) == 1) {
                    // the pid is present, congratulations
                    valid_pid_found = 1;

                    // remove the pid occurrence from the presence list
                    res = gnl_list_delete(&(waiting_list->presence_list), &(popped_el->pid), compare_int, destroy_pointer);

                    if (res == -1) {
                        // free memory
                        free(target_copy);

                        GNL_FSS_LOCK_RELEASE(NULL)

                        // let the errno bubble

                        return NULL;
                    }
                }
                // else free memory
                else {
                    destroy_el(popped_el);
                }

                // continue iterating
            };

            // stop searching
            break;
        }

        // move to the next element
        current = current->next;
    }

    // free memory
    free(target_copy);

    // release the lock
    GNL_FSS_LOCK_RELEASE(NULL)

    return popped_el;
}

/**
 * {@inheritDoc}
 */
int gnl_fss_waiting_list_remove(struct gnl_fss_waiting_list *waiting_list, int pid) {
    // acquire the lock
    GNL_FSS_LOCK_ACQUIRE(-1)

    // check the parameters
    GNL_FSS_NULL_CHECK(waiting_list, EINVAL, -1)

    int res;

    // delete all the pid occurrences from the presence list
    while (gnl_list_search(waiting_list->presence_list, &pid, compare_int) == 1) {
        res = gnl_list_delete(&(waiting_list->presence_list), &pid, compare_int, destroy_pointer);
        GNL_FSS_MINUS1_CHECK(res, errno, -1);
    }

    // release the lock
    GNL_FSS_LOCK_RELEASE(-1)

    return 0;
}

#undef GNL_FSS_LOCK_ACQUIRE
#undef GNL_FSS_LOCK_RELEASE
#undef GNL_FSS_COMPARE
#undef GNL_FSS_NULL_CHECK
#undef GNL_FSS_MINUS1_CHECK

#include <gnl_macro_end.h>