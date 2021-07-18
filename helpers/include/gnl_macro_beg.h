#ifndef GNL_MACRO_BEG_H
#define GNL_MACRO_BEG_H

#include <stdlib.h>
#include <errno.h>

#define GNL_CALLOC(ptr, len, return_code) {         \
    (ptr) = (char *)calloc((len), sizeof(char));    \
    if ((ptr) == NULL) {                            \
        errno = ENOMEM;                             \
                                                    \
        return return_code;                         \
    }                                               \
}

#define GNL_NULL_CHECK(x, error_code, return_code) {    \
    if ((x) == NULL) {                                  \
        errno = error_code;                             \
                                                        \
        return return_code;                             \
    }                                                   \
}

#define GNL_MINUS1_CHECK(x, error_code, return_code) {  \
    if ((x) == -1) {                                    \
        errno = error_code;                             \
                                                        \
        return return_code;                             \
    }                                                   \
}

#define GNL_TO_INT(value, string, return_code) {    \
    char *ptr = NULL;                               \
    (value) = (int)strtol(string, &ptr, 10);        \
                                                    \
    /* if no digits found */                        \
    if ((string) == ptr) {                          \
        errno = EINVAL;                             \
        return return_code;                         \
    }                                               \
                                                    \
    /* if there was an error */                     \
    if (errno != 0) {                               \
        return return_code;                         \
    }                                               \
}

#endif //GNL_MACRO_BEG_H