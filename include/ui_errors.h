#ifndef UI_ERRORS_H
#define UI_ERRORS_H

#include <stdbool.h>

/*
 * UI-level error codes.
 * These represent user-facing failures only.
 */
typedef enum {
    UI_OK = 0,

    UI_ERR_INPUT_TOO_LONG,
    UI_ERR_INVALID_MENU,
    UI_ERR_INVALID_CODE,
    UI_ERR_INVALID_WEIGHT,
    UI_ERR_INVALID_GRADE,
    UI_ERR_DUPLICATE,
    UI_ERR_NOT_FOUND,
    UI_ERR_EMPTY,
    UI_ERR_FILE_NOT_FOUND,
    UI_ERR_FILE_TYPE,
    UI_ERR_OOM
} ui_error_t;

/*
 * Print a formatted error message to stderr.
 * Does NOT exit.
 * Does NOT control flow.
 */
void ui_print_error(ui_error_t err);

/*
 * Check for input overflow (missing '\n').
 * If overflowed:
 *  - prints error
 *  - flushes stdin
 *  - returns true
 *
 * Otherwise returns false.
 */
bool ui_handle_long_input(const char *buf);

#endif /* UI_ERRORS_H */
