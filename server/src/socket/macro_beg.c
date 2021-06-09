#include <stdlib.h>

#define GNL_ALLOCATE_MESSAGE(ptr, len) {        \
    ptr = (char *)malloc(len * sizeof(char));   \
    if (ptr == NULL) {                          \
        errno = ENOMEM;                         \
                                                \
        return -1;                              \
    }                                           \
}

#define GNL_NULL_CHECK(x, error_code, return_code) {    \
    if (x == NULL) {                                    \
        errno = error_code;                             \
                                                        \
        return return_code;                             \
    }                                                   \
}

#define GNL_MINUS1_CHECK(x, error_code, return_code) {  \
    if (x == -1) {                                      \
        errno = error_code;                             \
                                                        \
        return return_code;                             \
    }                                                   \
}

#define CONV_TO_INT(from, to) { \
    to = atoi(from);            \
    if (to == 0) {              \
        errno = EINVAL;         \
        return -1;              \
    }                           \
}
