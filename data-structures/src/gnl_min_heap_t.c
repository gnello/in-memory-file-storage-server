/*
 * This is a simple min heap implementation, it does not intend to be exhaustive
 * but it's nice :)
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "../includes/gnl_min_heap_t.h"

// the node of the min heap
struct gnl_min_heap_node {
    void *data;
    int key;
};

// the min heap
struct gnl_min_heap_t {
    unsigned long size;
    struct gnl_min_heap_node *list;
};

/**
 * Get the parent node of i-element.
 *
 * @param i     The index of the element.
 * @return int  The parent index of the i-element.
 */
static int parent(int i) {
    return (i-1)/2;
}

/**
 * Get the left node of i-element.
 *
 * @param i     The index of the element.
 * @return int  The left index of the i-element.
 */
static int left(int i) {
    return (2*i)+1;
}

/**
 * Get the right node of i-element.
 *
 * @param i     The index of the element.
 * @return int  The right index of the i-element.
 */
static int right(int i) {
    return (2*i) + 2;
}

/**
 * Swap the i-element with the j-element.
 *
 * @param mh    The min heap where to swap the elements.
 * @param i     The first element.
 * @param j     The second element.
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
static int swap(gnl_min_heap_t *mh, int i, int j) {
    struct gnl_min_heap_node tmp = *(mh->list + i);
    *(mh->list + i) = *(mh->list + j);
    *(mh->list + j) = tmp;

    return 0;
}

/**
 * Reorder the min heap elements to respect the min heap property.
 *
 * @param mh    The min heap where to restore the min heap property.
 * @param i     The index of the element from where to start the "heapify".
 *
 * @return int  Returns 0 on success, -1 otherwise.
 */
static int min_heapify(gnl_min_heap_t *mh, int i) {
    int l = left(i);
    int r = right(i);
    int smallest = i;

    if (l < mh->size && (*(mh->list + l)).key < (*(mh->list + i)).key) {
        smallest = l;
    }

    if (r < mh->size && (*(mh->list + r)).key < (*(mh->list + smallest)).key) {
        smallest = r;
    }

    if (smallest != i) {
        swap(mh, i, smallest);
        min_heapify(mh, smallest);
    }

    return 0;
}

gnl_min_heap_t *gnl_min_heap_init() {
    gnl_min_heap_t *mh = (struct gnl_min_heap_t *)malloc(sizeof(struct gnl_min_heap_t));

    if (mh == NULL) {
        perror("malloc");

        return NULL;
    }

    // init the min heap implementation data
    mh->list = NULL;
    mh->size = 0;

    return mh;
}

void gnl_min_heap_destroy(gnl_min_heap_t *mh) {
    if (mh != NULL) {
        if (mh->list != NULL) {
            free(mh->list);
        }
        free(mh);
    }
}

int gnl_min_heap_insert(gnl_min_heap_t *mh, void *el, int key) {
    // allocate space for the new node of the min heap
    struct gnl_min_heap_node *temp;
    temp = realloc(mh->list, (mh->size + 1) * sizeof(struct gnl_min_heap_node));

    if (temp == NULL) {
        perror("realloc");

        return -1;
    }

    mh->list = temp;

    struct gnl_min_heap_node node;
    node.data = el;
    node.key = INT_MAX;

    *(mh->list + mh->size) = node;

    if (gnl_min_heap_decrease_key(mh, mh->size, key) != 0) {
        // errno bubble
        return -1;
    }

    mh->size++;

    return 0;
}

void *gnl_min_heap_extract_min(gnl_min_heap_t *mh) {
    if (mh->size < 1) {
        errno = EPERM;

        return NULL;
    }

    void *min = (*(mh->list)).data;

    mh->size--;
    *(mh->list) = *(mh->list + mh->size);

    min_heapify(mh, 0);

    return min;
}

int gnl_min_heap_decrease_key(gnl_min_heap_t *mh, int i, int key) {
    if (key > (*(mh->list + i)).key) {
        errno = EINVAL;

        return -1;
    }

    (*(mh->list + i)).key = key;

    while (i > 0 && (*(mh->list + parent(i))).key > (*(mh->list + i)).key) {
        swap(mh, i, parent(i));
        i = parent(i);
    }

    return 0;
}
