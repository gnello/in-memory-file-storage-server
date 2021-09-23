#include <stdio.h>
#include <gnl_colorshell.h>
#include <gnl_assert.h>
#include "../src/gnl_simfs_monitor.c"

int can_init_monitor() {
    struct gnl_simfs_monitor *monitor = gnl_simfs_monitor_init();

    if (monitor == NULL) {
        return -1;
    }

    if (monitor->file_peak != 0) {
        return -1;
    }

    if (monitor->bytes_peak != 0) {
        return -1;
    }

    if (monitor->file_counter != 0) {
        return -1;
    }

    if (monitor->bytes_counter != 0) {
        return -1;
    }

    if (monitor->file_evictions != 0) {
        return -1;
    }

    gnl_simfs_monitor_destroy(monitor);

    return 0;
}

int can_add_file() {
    struct gnl_simfs_monitor *monitor = gnl_simfs_monitor_init();
    if (monitor == NULL) {
        return -1;
    }

    int res = gnl_simfs_monitor_file_added(monitor);

    if (res != 0) {
        return -1;
    }

    if (monitor->file_counter != 1) {
        return -1;
    }

    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);

    if (monitor->file_counter != 8) {
        return -1;
    }

    gnl_simfs_monitor_destroy(monitor);

    return 0;
}

int can_remove_file() {
    struct gnl_simfs_monitor *monitor = gnl_simfs_monitor_init();
    if (monitor == NULL) {
        return -1;
    }

    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);

    if (monitor->file_counter != 7) {
        return -1;
    }

    gnl_simfs_monitor_file_removed(monitor);
    gnl_simfs_monitor_file_removed(monitor);
    gnl_simfs_monitor_file_removed(monitor);

    if (monitor->file_counter != 4) {
        return -1;
    }

    gnl_simfs_monitor_file_removed(monitor);
    gnl_simfs_monitor_file_removed(monitor);
    gnl_simfs_monitor_file_removed(monitor);

    if (monitor->file_counter != 1) {
        return -1;
    }

    gnl_simfs_monitor_destroy(monitor);

    return 0;
}

int can_not_remove_file() {
    struct gnl_simfs_monitor *monitor = gnl_simfs_monitor_init();
    if (monitor == NULL) {
        return -1;
    }

    int res = gnl_simfs_monitor_file_removed(monitor);

    if (res != -1) {
        return -1;
    }

    if (errno != EPERM) {
        return -1;
    }

    gnl_simfs_monitor_destroy(monitor);

    return 0;
}

int can_peak_file() {
    struct gnl_simfs_monitor *monitor = gnl_simfs_monitor_init();
    if (monitor == NULL) {
        return -1;
    }

    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);

    if (monitor->file_peak != 3) {
        return -1;
    }

    gnl_simfs_monitor_file_removed(monitor);
    gnl_simfs_monitor_file_removed(monitor);

    if (monitor->file_peak != 3) {
        return -1;
    }

    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);

    if (monitor->file_peak != 4) {
        return -1;
    }

    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);

    if (monitor->file_peak != 10) {
        return -1;
    }

    gnl_simfs_monitor_file_removed(monitor);
    gnl_simfs_monitor_file_removed(monitor);
    gnl_simfs_monitor_file_removed(monitor);
    gnl_simfs_monitor_file_removed(monitor);

    gnl_simfs_monitor_file_added(monitor);
    gnl_simfs_monitor_file_added(monitor);

    if (monitor->file_peak != 10) {
        return -1;
    }

    gnl_simfs_monitor_destroy(monitor);

    return 0;
}

int can_add_bytes() {
    struct gnl_simfs_monitor *monitor = gnl_simfs_monitor_init();
    if (monitor == NULL) {
        return -1;
    }

    int res = gnl_simfs_monitor_bytes_added(monitor, 546);

    if (res != 0) {
        return -1;
    }

    if (monitor->bytes_counter != 546) {
        return -1;
    }

    gnl_simfs_monitor_bytes_added(monitor, 1569);
    gnl_simfs_monitor_bytes_added(monitor, 406);
    gnl_simfs_monitor_bytes_added(monitor, 6987);
    gnl_simfs_monitor_bytes_added(monitor, 1);
    gnl_simfs_monitor_bytes_added(monitor, 0);
    gnl_simfs_monitor_bytes_added(monitor, 3216);
    gnl_simfs_monitor_bytes_added(monitor, 1024);

    if (monitor->bytes_counter != 13749) {
        return -1;
    }

    gnl_simfs_monitor_destroy(monitor);

    return 0;
}

int can_remove_bytes() {
    struct gnl_simfs_monitor *monitor = gnl_simfs_monitor_init();
    if (monitor == NULL) {
        return -1;
    }

    gnl_simfs_monitor_bytes_added(monitor, 546);

    if (monitor->bytes_counter != 546) {
        return -1;
    }

    gnl_simfs_monitor_bytes_removed(monitor, 1);
    gnl_simfs_monitor_bytes_removed(monitor, 203);
    gnl_simfs_monitor_bytes_removed(monitor, 44);

    if (monitor->bytes_counter != 298) {
        return -1;
    }

    gnl_simfs_monitor_bytes_removed(monitor, 56);
    gnl_simfs_monitor_bytes_removed(monitor, 98);
    gnl_simfs_monitor_bytes_removed(monitor, 23);

    if (monitor->bytes_counter != 121) {
        return -1;
    }

    gnl_simfs_monitor_destroy(monitor);

    return 0;
}

int can_not_remove_bytes() {
    struct gnl_simfs_monitor *monitor = gnl_simfs_monitor_init();
    if (monitor == NULL) {
        return -1;
    }

    int res = gnl_simfs_monitor_bytes_removed(monitor, 53);

    if (res != -1) {
        return -1;
    }

    if (errno != EPERM) {
        return -1;
    }

    gnl_simfs_monitor_destroy(monitor);

    return 0;
}

int can_peak_bytes() {
    struct gnl_simfs_monitor *monitor = gnl_simfs_monitor_init();
    if (monitor == NULL) {
        return -1;
    }

    gnl_simfs_monitor_bytes_added(monitor, 23);
    gnl_simfs_monitor_bytes_added(monitor, 21);
    gnl_simfs_monitor_bytes_added(monitor, 156);

    if (monitor->bytes_peak != 200) {
        return -1;
    }

    gnl_simfs_monitor_bytes_removed(monitor, 33);
    gnl_simfs_monitor_bytes_removed(monitor, 56);

    if (monitor->bytes_peak != 200) {
        return -1;
    }

    gnl_simfs_monitor_bytes_added(monitor, 450);
    gnl_simfs_monitor_bytes_added(monitor, 789);
    gnl_simfs_monitor_bytes_added(monitor, 63);

    if (monitor->bytes_peak != 1413) {
        return -1;
    }

    gnl_simfs_monitor_bytes_added(monitor, 20);
    gnl_simfs_monitor_bytes_added(monitor, 36);
    gnl_simfs_monitor_bytes_added(monitor, 98);
    gnl_simfs_monitor_bytes_added(monitor, 158);
    gnl_simfs_monitor_bytes_added(monitor, 195);
    gnl_simfs_monitor_bytes_added(monitor, 7894);

    if (monitor->bytes_peak != 9814) {
        return -1;
    }

    gnl_simfs_monitor_bytes_removed(monitor, 536);
    gnl_simfs_monitor_bytes_removed(monitor, 1457);
    gnl_simfs_monitor_bytes_removed(monitor, 369);
    gnl_simfs_monitor_bytes_removed(monitor, 45);

    gnl_simfs_monitor_bytes_added(monitor, 3);
    gnl_simfs_monitor_bytes_added(monitor, 405);

    if (monitor->bytes_peak != 9814) {
        return -1;
    }

    gnl_simfs_monitor_destroy(monitor);

    return 0;
}

int can_add_evictions() {
    struct gnl_simfs_monitor *monitor = gnl_simfs_monitor_init();
    if (monitor == NULL) {
        return -1;
    }

    int res = gnl_simfs_monitor_eviction_started(monitor);

    if (res != 0) {
        return -1;
    }

    if (monitor->file_evictions != 1) {
        return -1;
    }

    gnl_simfs_monitor_eviction_started(monitor);
    gnl_simfs_monitor_eviction_started(monitor);
    gnl_simfs_monitor_eviction_started(monitor);
    gnl_simfs_monitor_eviction_started(monitor);
    gnl_simfs_monitor_eviction_started(monitor);
    gnl_simfs_monitor_eviction_started(monitor);
    gnl_simfs_monitor_eviction_started(monitor);

    if (monitor->file_evictions != 8) {
        return -1;
    }

    gnl_simfs_monitor_destroy(monitor);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_simfs_monitor test:\n\n");

    gnl_assert(can_init_monitor, "can init a monitor.");

    gnl_assert(can_add_file, "can track a file addition.");
    gnl_assert(can_remove_file, "can track a file removal.");
    gnl_assert(can_not_remove_file, "can not track inconsistent file removal.");
    gnl_assert(can_peak_file, "can track the file peak.");

    gnl_assert(can_add_bytes, "can track bytes addition.");
    gnl_assert(can_remove_bytes, "can track bytes removal.");
    gnl_assert(can_not_remove_bytes, "can not track inconsistent bytes removal.");
    gnl_assert(can_peak_bytes, "can track the bytes peak.");

    gnl_assert(can_add_evictions, "can track file evictions.");

    // the gnl_simfs_monitor_destroy method is implicitly tested in every assertion

    printf("\n");
}