/***********************************
 * main.c
 *
 * CGPA Calculator - program entry point and user interaction
 *
 * Handles:
 * - menu loop
 * - user input
 * - program flow control
 *
 * Author: Arul Rao (Vidonicle)
 * License: MIT
 ***********************************/

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgpa.h"
#include "ui_errors.h"

static void menu_load_from_file(calculator_t *calculator, int argc, char *argv[]);
static menu_option_t menu_get_choice(calculator_t *calculator);
static void menu_add_course(calculator_t *calculator);
static void menu_edit_course(calculator_t *calculator);
static void menu_delete_course(calculator_t *calculator);

int main(int argc, char *argv[]) {
    calculator_t calculator;

    initialize_courses(&calculator.courses);
    menu_load_from_file(&calculator, argc, argv);

    // Menu loop
    while (true) {
        print_menu();

        switch (menu_get_choice(&calculator)) {
            case MENU_ADD:
                menu_add_course(&calculator);
                break;

            case MENU_EDIT:
                menu_edit_course(&calculator);
                break;

            case MENU_DELETE:
                menu_delete_course(&calculator);
                break;

            case MENU_DISPLAY:
                display_grades(&calculator.courses);
                break;

            case MENU_EXIT:
                printf("\n  Goodbye!\n");
                teardown(&calculator.courses);
                return EXIT_SUCCESS;

            case MENU_INVALID:
                ui_print_error(UI_ERR_INVALID_MENU);
                break;

            default:
                break;
        }
    }
}

bool ui_handle_input(course_list_t *courses, char *buf, size_t size) {
    if (!fgets((buf), size, stdin)) {
        int status = EXIT_SUCCESS;

        if (ferror(stdin)) {
            perror("Error reading input");
            status = EXIT_FAILURE;
        }

        teardown(courses);
        exit(status);
    }

    return ui_handle_long_input(buf);
}

void menu_load_from_file(calculator_t *calculator, int argc, char *argv[]) {
    bool load_file = true;
    FILE *fptr = NULL;

    if (argc == 2) {
        char exec_path[PATH_MAX];
        if (realpath(argv[0], exec_path)) {
            char *slash = strrchr(exec_path, '/');
            if (slash) {
                *slash = '\0';  // strip executable name
            }

            char pathbuf[PATH_MAX];
            snprintf(pathbuf, sizeof(pathbuf), "%s/../data/%s", exec_path, argv[1]);
            fptr = fopen(pathbuf, "r");
            if (!fptr) {
                ui_print_error(UI_ERR_FILE_NOT_FOUND);
                return;
            }
        }
    } else {
        while (true) {
            printf(SEPERATOR1 "\n  Would you like to load calculator->courses from file? (Y/n): ");

            load_file = ui_handle_input(&calculator->courses, calculator->input_buffers.menu_buf,
                                        sizeof(calculator->input_buffers.menu_buf));

            calculator->input_buffers.menu_buf[strcspn(calculator->input_buffers.menu_buf, "\n")] =
                '\0';
            calculator->input_buffers.menu_buf[0] =
                (char)tolower((unsigned char)calculator->input_buffers.menu_buf[0]);

            if (calculator->input_buffers.menu_buf[0] == 'n')
                load_file = false;

            if (load_file) {
                // File parsing loop
                printf(SEPERATOR1 "\n  Please enter the name of your file: ");

                if (!ui_handle_input(&calculator->courses, calculator->input_buffers.filename,
                                     sizeof(calculator->input_buffers.filename)))
                    continue;

                calculator->input_buffers
                    .filename[strcspn(calculator->input_buffers.filename, "\n")] = '\0';
                if (calculator->input_buffers.filename[0] == '\0')
                    continue;

                // Locate file in intended folder
                char exec_path[PATH_MAX];
                if (realpath(argv[0], exec_path)) {
                    char *slash = strrchr(exec_path, '/');
                    if (slash) {
                        *slash = '\0';  // strip executable name
                    }
                    char pathbuf[PATH_MAX];
                    snprintf(pathbuf, sizeof(pathbuf), "%s/../data/%s", exec_path,
                             calculator->input_buffers.filename);
                    fptr = fopen(pathbuf, "r");
                }
            } else
                return;

            if (!fptr) {
                ui_print_error(UI_ERR_FILE_NOT_FOUND);
            } else
                break;
        }
    }

    if (!load_from_file(&calculator->courses, fptr)) {
        fclose(fptr);
        ui_print_error(UI_ERR_OOM);
        teardown(&calculator->courses);
        exit(EXIT_FAILURE);
    } else {
        printf(SEPERATOR1 "\n  Load from file successful!\n");
        fclose(fptr);
    }
}

menu_option_t menu_get_choice(calculator_t *calculator) {
    if (!ui_handle_input(&calculator->courses, calculator->input_buffers.menu_buf,
                         sizeof(calculator->input_buffers.menu_buf)))
        return MENU_INVALID;

    calculator->input_buffers.menu_buf[strcspn(calculator->input_buffers.menu_buf, "\n")] = '\0';

    char *m_endptr;
    errno = 0;

    long menu_choice = strtol(calculator->input_buffers.menu_buf, &m_endptr, 10);

    if (errno == ERANGE || m_endptr == calculator->input_buffers.menu_buf || *m_endptr != '\0' ||
        menu_choice < MENU_ADD || menu_choice >= MENU_COUNT)
        return MENU_INVALID;

    return (menu_option_t)menu_choice;
}

void menu_add_course(calculator_t *calculator) {
    do {
        // Course Code
        printf(SEPERATOR1 "  Please enter your course code (Ex. SYSC2006): ");

        if (!ui_handle_input(&calculator->courses, calculator->input_buffers.course_code_buf,
                             sizeof(calculator->input_buffers.course_code_buf)))
            break;

        calculator->input_buffers
            .course_code_buf[strcspn(calculator->input_buffers.course_code_buf, "\n")] = '\0';

        if (!validate_course_code(calculator->input_buffers.course_code_buf)) {
            ui_print_error(UI_ERR_INVALID_CODE);
            break;
        }

        if (check_courses(&calculator->courses, calculator->input_buffers.course_code_buf)) {
            ui_print_error(UI_ERR_DUPLICATE);
            break;
        }

        // Course Weight
        printf(SEPERATOR2 "  Please enter your course weight (1.00, 0.50, 0.25, 0.00): ");

        if (!ui_handle_input(&calculator->courses, calculator->input_buffers.course_weight_buf,
                             sizeof(calculator->input_buffers.course_weight_buf)))
            break;

        calculator->input_buffers
            .course_weight_buf[strcspn(calculator->input_buffers.course_weight_buf, "\n")] = '\0';

        char *cw_endptr;
        float course_weight = strtof(calculator->input_buffers.course_weight_buf, &cw_endptr);

        if (*cw_endptr != '\0' || !validate_course_weight(course_weight)) {
            ui_print_error(UI_ERR_INVALID_WEIGHT);
            break;
        }

        // Letter Grade
        printf(SEPERATOR2
               "  Please enter your letter grade ((A, B, C, "
               "D)+/-, F, CR, NR, or SAT): ");

        if (!ui_handle_input(&calculator->courses, calculator->input_buffers.letter_grade_buf,
                             sizeof(calculator->input_buffers.letter_grade_buf)))
            break;

        calculator->input_buffers
            .letter_grade_buf[strcspn(calculator->input_buffers.letter_grade_buf, "\n")] = '\0';

        for (size_t i = 0; calculator->input_buffers.letter_grade_buf[i] != '\0'; i++)
            calculator->input_buffers.letter_grade_buf[i] =
                (char)toupper((unsigned char)calculator->input_buffers.letter_grade_buf[i]);

        if (!validate_letter_grade(calculator->input_buffers.letter_grade_buf, course_weight)) {
            ui_print_error(UI_ERR_INVALID_GRADE);
            break;
        }

        if (!add_course(&calculator->courses, calculator->input_buffers.course_code_buf,
                        course_weight, calculator->input_buffers.letter_grade_buf)) {
            ui_print_error(UI_ERR_OOM);
            teardown(&calculator->courses);
            exit(EXIT_FAILURE);
        } else {
            printf(SEPERATOR2
                   "\n  Course Successfully Added!\n"
                   "  -Course Code: %s\n"
                   "  -Course Weight: %4.2f\n"
                   "  -Letter Grade: %s\n",
                   calculator->input_buffers.course_code_buf, course_weight,
                   calculator->input_buffers.letter_grade_buf);
        }
    } while (0);
}

void menu_edit_course(calculator_t *calculator) {
    do {
        // Get Old course code
        printf(SEPERATOR1
               "  Please enter the course code for the course you "
               "want to edit (Ex. SYSC2006): ");

        if (!ui_handle_input(&calculator->courses, calculator->input_buffers.course_code_buf,
                             sizeof(calculator->input_buffers.course_code_buf)))
            break;

        calculator->input_buffers
            .course_code_buf[strcspn(calculator->input_buffers.course_code_buf, "\n")] = '\0';

        if (!validate_course_code(calculator->input_buffers.course_code_buf)) {
            ui_print_error(UI_ERR_INVALID_CODE);
            break;
        }

        if (!check_courses(&calculator->courses, calculator->input_buffers.course_code_buf)) {
            ui_print_error(UI_ERR_COURSE_NOT_FOUND);
            break;
        }

        char course_code_old[COURSE_CODE_BUF_LEN];
        strcpy(course_code_old, calculator->input_buffers.course_code_buf);

        coursenode_t *fetched_node =
            fetch_node(&calculator->courses, calculator->input_buffers.course_code_buf);
        float old_weight = fetched_node->course_weight;
        char old_grade[LETTER_GRADE_BUF_LEN];
        strcpy(old_grade, fetched_node->letter_grade);

        // Get new course code
        printf(SEPERATOR2 "  Enter new course code (leave blank for no changes): ");

        if (!ui_handle_input(&calculator->courses, calculator->input_buffers.course_code_buf,
                             sizeof(calculator->input_buffers.course_code_buf)))
            break;

        calculator->input_buffers
            .course_code_buf[strcspn(calculator->input_buffers.course_code_buf, "\n")] = '\0';

        char course_code_new[COURSE_CODE_BUF_LEN];
        if (calculator->input_buffers.course_code_buf[0] == '\0') {
            strcpy(course_code_new, course_code_old);
        } else {
            if (!validate_course_code(calculator->input_buffers.course_code_buf)) {
                ui_print_error(UI_ERR_INVALID_CODE);
                break;
            }
            strcpy(course_code_new, calculator->input_buffers.course_code_buf);
        }

        if (strcmp(course_code_new, course_code_old) != 0 &&
            check_courses(&calculator->courses, course_code_new)) {
            ui_print_error(UI_ERR_DUPLICATE);
            break;
        }

        // Get course weight
        printf(SEPERATOR2 "  Enter new course weight (leave blank for no changes): ");

        if (!ui_handle_input(&calculator->courses, calculator->input_buffers.course_weight_buf,
                             sizeof(calculator->input_buffers.course_weight_buf)))
            break;

        calculator->input_buffers
            .course_weight_buf[strcspn(calculator->input_buffers.course_weight_buf, "\n")] = '\0';

        float course_weight_new = old_weight;

        if (calculator->input_buffers.course_weight_buf[0] != '\0') {
            char *cwn_endptr;
            float course_weight = strtof(calculator->input_buffers.course_weight_buf, &cwn_endptr);

            if (*cwn_endptr != '\0' || !validate_course_weight(course_weight)) {
                ui_print_error(UI_ERR_INVALID_WEIGHT);
                break;
            }

            course_weight_new = course_weight;
        }

        printf(SEPERATOR2 "  Enter new letter grade (leave blank for no changes): ");

        if (!ui_handle_input(&calculator->courses, calculator->input_buffers.letter_grade_buf,
                             sizeof(calculator->input_buffers.letter_grade_buf)))
            break;

        calculator->input_buffers
            .letter_grade_buf[strcspn(calculator->input_buffers.letter_grade_buf, "\n")] = '\0';

        char letter_grade_new[LETTER_GRADE_BUF_LEN];
        if (calculator->input_buffers.letter_grade_buf[0] == '\0') {
            strcpy(letter_grade_new, old_grade);
        } else {
            for (size_t i = 0; calculator->input_buffers.letter_grade_buf[i] != '\0'; i++)
                calculator->input_buffers.letter_grade_buf[i] =
                    (char)toupper((unsigned char)calculator->input_buffers.letter_grade_buf[i]);

            if (!validate_letter_grade(calculator->input_buffers.letter_grade_buf,
                                       course_weight_new)) {
                ui_print_error(UI_ERR_INVALID_GRADE);
                break;
            }
            strcpy(letter_grade_new, calculator->input_buffers.letter_grade_buf);
        }

        // Finalize edit
        if (!edit_course(&calculator->courses, course_code_old, course_code_new, course_weight_new,
                         letter_grade_new)) {
            ui_print_error(UI_ERR_OOM);
            teardown(&calculator->courses);
            exit(EXIT_FAILURE);
        }
        printf(SEPERATOR2
               "\n  Course Successfully Edited!\n"
               "  -Old Course Code: %s\n"
               "  -New Course Code: %s\n"
               "  -Course Weight: %4.2f\n"
               "  -Letter Grade: %s\n",
               course_code_old, course_code_new, course_weight_new, letter_grade_new);
    } while (0);
}

void menu_delete_course(calculator_t *calculator) {
    do {
        if (calculator->courses.size == 0) {
            ui_print_error(UI_ERR_EMPTY);
            continue;
        }

        printf(SEPERATOR1
               "  Please enter the course code for the course you "
               "want to delete (Ex. SYSC2006): ");

        if (!ui_handle_input(&calculator->courses, calculator->input_buffers.course_code_buf,
                             sizeof(calculator->input_buffers.course_code_buf)))
            break;

        calculator->input_buffers
            .course_code_buf[strcspn(calculator->input_buffers.course_code_buf, "\n")] = '\0';

        if (!validate_course_code(calculator->input_buffers.course_code_buf)) {
            ui_print_error(UI_ERR_INVALID_CODE);
        } else {
            if (!check_courses(&calculator->courses, calculator->input_buffers.course_code_buf)) {
                ui_print_error(UI_ERR_COURSE_NOT_FOUND);
            } else {
                delete_course(&calculator->courses, calculator->input_buffers.course_code_buf);
                printf(SEPERATOR2 "\n  Course successfully deleted\n");
            }
        }
    } while (0);
}
