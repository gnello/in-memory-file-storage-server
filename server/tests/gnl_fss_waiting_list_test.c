#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_list_t.h>
#include "../src/gnl_fss_waiting_list.c"

int can_init_a_waiting_list() {
    struct gnl_fss_waiting_list *wl = gnl_fss_waiting_list_init();

    if (wl->target_list != NULL) {
        return -1;
    }

    if (wl->presence_list != NULL) {
        return -1;
    }

    gnl_fss_waiting_list_destroy(wl);

    return 0;
}

int can_push_new() {
    struct gnl_fss_waiting_list *wl = gnl_fss_waiting_list_init();

    int target = 1;
    int pid = 6;

    int res = gnl_fss_waiting_list_push(wl, target, pid);

    if (res == -1) {
        return -1;
    }

    if (gnl_list_search(wl->presence_list, &pid, compare_int) == 0) {
        return -1;
    }

    // get the existent target element
    struct gnl_list_t *current = wl->target_list;

    int found = 0;

    while (current != NULL) {
        // if target is found
        if (((struct gnl_fss_waiting_list_el *)(current->el))->target == target) {
            void *enqueued_pid_raw = gnl_queue_dequeue(((struct gnl_fss_waiting_list_el *)(current->el))->queue);

            if (enqueued_pid_raw == NULL) {
                return -1;
            }

            int enqueued_pid = *(int *)enqueued_pid_raw;

            free(enqueued_pid_raw);

            if (enqueued_pid != pid) {
                return -1;
            }

            found = 1;

            // stop searching
            break;
        }
    }

    if (found != 1) {
        return -1;
    }

    gnl_fss_waiting_list_destroy(wl);

    return 0;
}

int can_push_new_two() {
    struct gnl_fss_waiting_list *wl = gnl_fss_waiting_list_init();

    int target = 1;
    int pid1 = 6;
    int pid2 = 5;

    int res = gnl_fss_waiting_list_push(wl, target, pid1);

    if (res == -1) {
        return -1;
    }

    res = gnl_fss_waiting_list_push(wl, target, pid2);

    if (res == -1) {
        return -1;
    }

    if (gnl_list_search(wl->presence_list, &pid1, compare_int) == 0) {
        return -1;
    }

    if (gnl_list_search(wl->presence_list, &pid2, compare_int) == 0) {
        return -1;
    }

    // get the existent target element
    struct gnl_list_t *current = wl->target_list;

    int found = 0;

    while (current != NULL) {
        // if target is found
        if (((struct gnl_fss_waiting_list_el *)(current->el))->target == target) {
            void *enqueued_pid_raw1 = gnl_queue_dequeue(((struct gnl_fss_waiting_list_el *)(current->el))->queue);

            if (enqueued_pid_raw1 == NULL) {
                return -1;
            }


            void *enqueued_pid_raw2 = gnl_queue_dequeue(((struct gnl_fss_waiting_list_el *)(current->el))->queue);

            if (enqueued_pid_raw2 == NULL) {
                return -1;
            }

            int enqueued_pid1 = *(int *)enqueued_pid_raw1;
            int enqueued_pid2 = *(int *)enqueued_pid_raw2;

            free(enqueued_pid_raw1);
            free(enqueued_pid_raw2);

            if (enqueued_pid1 != pid1) {
                return -1;
            }

            if (enqueued_pid2 != pid2) {
                return -1;
            }

            found = 1;

            // stop searching
            break;
        }

        current = current->next;
    }

    if (found != 1) {
        return -1;
    }

    gnl_fss_waiting_list_destroy(wl);

    return 0;
}

int can_push_new_two_different() {
    struct gnl_fss_waiting_list *wl = gnl_fss_waiting_list_init();

    int target1 = 1;
    int target2 = 2;
    int pid1 = 6;
    int pid2 = 5;

    int res = gnl_fss_waiting_list_push(wl, target1, pid1);

    if (res == -1) {
        return -1;
    }

    res = gnl_fss_waiting_list_push(wl, target2, pid2);

    if (res == -1) {
        return -1;
    }

    if (gnl_list_search(wl->presence_list, &pid1, compare_int) == 0) {
        return -1;
    }

    if (gnl_list_search(wl->presence_list, &pid2, compare_int) == 0) {
        return -1;
    }

    // get the existent target element
    struct gnl_list_t *current = wl->target_list;

    int found = 0;

    while (current != NULL) {
        struct gnl_fss_waiting_list_el * el = current->el;

        // if target is found
        if (el->target == target1 || el->target == target2) {
            void *enqueued_pid_raw = gnl_queue_dequeue(((struct gnl_fss_waiting_list_el *)(current->el))->queue);

            if (enqueued_pid_raw == NULL) {
                return -1;
            }

            int enqueued_pid = *(int *)enqueued_pid_raw;

            free(enqueued_pid_raw);

            if (el->target == target1 && enqueued_pid != pid1) {
                return -1;
            } else if (el->target == target2 && enqueued_pid != pid2) {
                return -1;
            } else if (el->target != target1 && el->target != target2) {
                return -1;
            }

            found++;
        }

        current = current->next;
    }

    if (found != 2) { printf("%d", found);
        return -1;
    }

    gnl_fss_waiting_list_destroy(wl);

    return 0;
}

int can_not_twice_presence() {
    struct gnl_fss_waiting_list *wl = gnl_fss_waiting_list_init();

    int pid = 6;

    int res = gnl_fss_waiting_list_push(wl, 1, pid);
    if (res == -1) {
        return -1;
    }

    res = gnl_fss_waiting_list_push(wl, 1, pid);
    if (res == -1) {
        return -1;
    }

    res = gnl_fss_waiting_list_push(wl, 1, pid);
    if (res == -1) {
        return -1;
    }

    res = gnl_fss_waiting_list_push(wl, 2, pid);
    if (res == -1) {
        return -1;
    }

    res = gnl_fss_waiting_list_push(wl, 3, pid);
    if (res == -1) {
        return -1;
    }

    if (gnl_list_search(wl->presence_list, &pid, compare_int) == 0) {
        return -1;
    }

    // get the existent target element
    struct gnl_list_t *current = wl->presence_list;

    int found = 0;

    while (current != NULL) {
        // if target is found
        if ((*(int *)(current->el)) == pid) {
            found++;
        }

        current = current->next;
    }

    if (found != 1) {
        return -1;
    }

    gnl_fss_waiting_list_destroy(wl);

    return 0;
}

int can_pop() {
    struct gnl_fss_waiting_list *wl = gnl_fss_waiting_list_init();

    int target = 1;
    int pid = 6;

    int res = gnl_fss_waiting_list_push(wl, 0, 0);
    if (res == -1) {
        return -1;
    }

    res = gnl_fss_waiting_list_push(wl, 4, 3);
    if (res == -1) {
        return -1;
    }

    res = gnl_fss_waiting_list_push(wl, target, pid);
    if (res == -1) {
        return -1;
    }

    int popped_pid = gnl_fss_waiting_list_pop(wl, target);

    if (popped_pid != pid) {
        return -1;
    }

    gnl_fss_waiting_list_destroy(wl);

    return 0;
}

int can_pop_empty() {
    struct gnl_fss_waiting_list *wl = gnl_fss_waiting_list_init();

    int popped_pid = gnl_fss_waiting_list_pop(wl, 4);

    if (popped_pid != -1) {
        return -1;
    }

    if (errno != 0) {
        return -1;
    }

    gnl_fss_waiting_list_destroy(wl);

    return 0;
}

int can_remove_pop() {
    struct gnl_fss_waiting_list *wl = gnl_fss_waiting_list_init();

    int target = 1;
    int pid = 6;

    int res = gnl_fss_waiting_list_push(wl, target, pid);
    if (res == -1) {
        return -1;
    }

    int popped_pid = gnl_fss_waiting_list_pop(wl, target);

    if (popped_pid != pid) {
        return -1;
    }

    popped_pid = gnl_fss_waiting_list_pop(wl, target);

    if (popped_pid == pid) {
        return -1;
    }

    if (gnl_list_search(wl->presence_list, &pid, compare_int) == 1) {
        return -1;
    }

    gnl_fss_waiting_list_destroy(wl);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_fss_waiting_list test:\n\n");

    gnl_assert(can_init_a_waiting_list, "can init a waiting list.");
    
    gnl_assert(can_push_new, "can push a new waiting pid to a target waiting list.");
    gnl_assert(can_push_new_two, "can push two new waiting pid to the same target waiting list.");
    gnl_assert(can_push_new_two_different, "can push two new waiting pid to different target waiting list.");
    gnl_assert(can_not_twice_presence, "can not be twice elements for the same pid into the presence channel.");

    gnl_assert(can_pop, "can pop any waiting pid from a target waiting list.");
    gnl_assert(can_pop_empty, "can pop from an empty target waiting list.");
    gnl_assert(can_remove_pop, "can remove a popped pid from a target waiting list.");
    // the gnl_fss_waiting_list_destroy method is implicitly tested in every assertion

    printf("\n");
}