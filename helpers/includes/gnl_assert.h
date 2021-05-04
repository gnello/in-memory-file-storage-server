
#ifndef GNL_ASSERT_H
#define GNL_ASSERT_H

/**
 * Check whether the return of *fun() is success or error
 * and print a green or red assert message depending on it,
 * followed by the fun_desc string.
 *
 * @param fun       The callable function of which check the result.
 * @param fun_desc  The message to print after the assert message.
 */
extern void gnl_assert(int (*fun)(), char *fun_desc);

#endif //GNL_ASSERT_H