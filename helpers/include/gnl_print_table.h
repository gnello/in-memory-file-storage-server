
#ifndef GNL_PRINT_TABLE_H
#define GNL_PRINT_TABLE_H

/**
 * Print two arguments in two columns.
 *
 * @param col1_string   The firs column value.
 * @param col2_string   The second column value.
 *
 * @return int          Returns the number of chars written
 *                      on success, -1 otherwise.
 */
extern int gnl_print_table(char *col1_string, char *col2_string);

#endif //GNL_PRINT_TABLE_H
