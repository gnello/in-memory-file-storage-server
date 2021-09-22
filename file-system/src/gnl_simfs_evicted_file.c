#include <errno.h>
#include <string.h>
#include "../include/gnl_simfs_evicted_file.h"
#include <gnl_macro_beg.h>

/**
 * {@inheritDoc}
 */
struct gnl_simfs_evicted_file *gnl_simfs_evicted_file_init() {
    struct gnl_simfs_evicted_file *evicted_file = malloc(sizeof(struct gnl_simfs_evicted_file));
    GNL_NULL_CHECK(evicted_file, ENOMEM, NULL)

    evicted_file->name = NULL;
    evicted_file->bytes = NULL;
    evicted_file->count = 0;

    return evicted_file;
}

/**
 * {@inheritDoc}
 */
void gnl_simfs_evicted_file_destroy(struct gnl_simfs_evicted_file *evicted_file) {
    if (evicted_file == NULL) {
        return;
    }

    free(evicted_file->name);
    free(evicted_file->bytes);
    free(evicted_file);
}

#include <gnl_macro_end.h>
