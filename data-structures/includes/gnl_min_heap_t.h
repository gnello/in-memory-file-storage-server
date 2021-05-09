
#ifndef GNL_MIN_HEAP_H
#define GNL_MIN_HEAP_H

typedef struct gnl_min_heap_t gnl_min_heap_t;

extern gnl_min_heap_t *gnl_min_heap_init();

extern void gnl_min_heap_destroy(gnl_min_heap_t *mh);

extern int gnl_min_heap_insert(gnl_min_heap_t *mh, void *el, int key);

extern void *gnl_min_heap_extract_min(gnl_min_heap_t *mh);

extern int gnl_min_heap_decrease_key(gnl_min_heap_t *mh, int i, int key);

#endif //GNL_MIN_HEAP_H