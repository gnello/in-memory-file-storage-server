#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include <gnl_list_t.h>
#include "../src/gnl_fss_waiting_list.c"

int compare_int(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

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

    if (gnl_list_search(wl->presence_list, &pid, compare_int) == 0) {printf("però");
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

int main() {
    gnl_printf_yellow("> gnl_fss_waiting_list test:\n\n");

    gnl_assert(can_init_a_waiting_list, "can init a waiting list.");
    
    gnl_assert(can_push_new, "can push a new waiting pid to a target waiting list.");

    // the gnl_fss_waiting_list_destroy method is implicitly tested in every assertion

    printf("\n");
}