#include "ui_errors.h"

#include <stdio.h>
#include <string.h>

#include "cgpa.h"  // for SEPERATORs and flush_stdin()

void ui_print_error(ui_error_t err) {
    fprintf(stderr, SEPERATOR2);

    switch (err) {
        case UI_ERR_INPUT_TOO_LONG:
            fprintf(stderr, "\n  Error: input is too long\n");
            break;

        case UI_ERR_INVALID_MENU:
            fprintf(stderr, "\n  Error: invalid menu choice\n");
            break;

        case UI_ERR_INVALID_CODE:
            fprintf(stderr, "\n  Error: invalid course code\n");
            break;

        case UI_ERR_INVALID_WEIGHT:
            fprintf(stderr, "\n  Error: invalid course weight\n");
            break;

        case UI_ERR_INVALID_GRADE:
            fprintf(stderr, "\n  Error: invalid letter grade\n");
            break;

        case UI_ERR_DUPLICATE:
            fprintf(stderr, "\n  Error: course already exists\n");
            break;

        case UI_ERR_NOT_FOUND:
            fprintf(stderr, "\n  Error: course not found\n");
            break;

        case UI_ERR_EMPTY:
            fprintf(stderr, "\n  Error: No courses in list");
            break;

        case UI_ERR_FILE_NOT_FOUND:
            fprintf(stderr, "\n  Error: file not found\n");
            break;

        case UI_ERR_FILE_TYPE:
            fprintf(stderr, "\n  Error: invalid file type\n");
            break;

        case UI_ERR_OOM:
            fprintf(stderr, "\n  Fatal error: out of memory\n");
            break;

        case UI_OK:
        default:
            break;
    }
}

bool ui_handle_long_input(const char *buf) {
    if (!strchr(buf, '\n')) {
        ui_print_error(UI_ERR_INPUT_TOO_LONG);
        flush_stdin();
        return true;
    }
    return false;
}