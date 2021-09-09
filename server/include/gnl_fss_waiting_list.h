
#ifndef GNL_FSS_WAITING_LIST_H
#define GNL_FSS_WAITING_LIST_H

/**
 * The waiting list structure.
 */
struct gnl_fss_waiting_list;

/**
 * Create a new waiting_list instance.
 *
 * @return  Returns the new waiting_list instance created on success,
 *          NULL otherwise.
 */
extern struct gnl_fss_waiting_list *gnl_fss_waiting_list_init();

/**
 * Destroy a waiting_list config.
 *
 * @param waiting_list  The waiting_list to be destroyed.
 */
extern void gnl_fss_waiting_list_destroy(struct gnl_fss_waiting_list *waiting_list);

/**
 * Push the given pid to the given target waiting list.
 *
 * @param waiting_list  The waiting_list instance.
 * @param target        The target where to push the pid.
 * @param pid           The pid to push.
 *
 * @return              Returns 0 on success, null otherwise.
 */
extern int gnl_fss_waiting_list_push(struct gnl_fss_waiting_list *waiting_list, int target, int pid);

/**
 * Pop a pid from the given target waiting list.
 *
 * @param waiting_list  The waiting_list instance.
 * @param target        The target from where to pop a pid.
 *
 * @return              Returns the pid popped on success,
 *                      -1 otherwise.
 */
extern int gnl_fss_waiting_list_pop(struct gnl_fss_waiting_list *waiting_list, int target);

/**
 * Remove the given pid from the every waiting list.
 *
 * @param waiting_list  The waiting_list instance.
 * @param pid           The pid to remove.
 *
 * @return              Returns 0 on success, null otherwise.
 */
extern int gnl_fss_waiting_list_remove(struct gnl_fss_waiting_list *waiting_list, int pid);

#endif //GNL_FSS_WAITING_LIST_H