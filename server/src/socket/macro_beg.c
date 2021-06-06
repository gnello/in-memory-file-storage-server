
#define GNL_ALLOCATE_MESSAGE(ptr, len) {        \
    ptr = (char *)malloc(len * sizeof(char));   \
    if (ptr == NULL) {                          \
        errno = ENOMEM;                         \
                                                \
        return -1;                              \
    }                                           \
}

#define GNL_NULL_CHECK(list, error_code, return_code) {     \
    if (list == NULL) {                                     \
        errno = error_code;                                 \
                                                            \
        return return_code;                                 \
    }                                                       \
}