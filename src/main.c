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
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgpa.h"
#include "ui_errors.h"

#define MENU_BUF_LEN 64            // 62 chars + '\n' + '\0'
#define FILENAME_LEN MENU_BUF_LEN  // Length of filename

int main(int argc, char *argv[]) {
    char course_code_buf[COURSE_CODE_BUF_LEN];
    char course_weight_buf[COURSE_WEIGHT_BUF_LEN];
    float course_weight = 0.0f;
    char letter_grade_buf[LETTER_GRADE_BUF_LEN];
    char filename[FILENAME_LEN];
    char menu_buf[MENU_BUF_LEN];

    list_courses_t courses;
    init_list(&courses);

    bool load_file = true;

    if (argc == 2) {
        FILE *fptr = fopen(argv[1], "r");

        if (!fptr) {
            ui_print_error(UI_ERR_FILE_NOT_FOUND);
        } else {
            if (!load_from_file(&courses, fptr)) {
                ui_print_error(UI_ERR_OOM);
                teardown(&courses);
                return EXIT_FAILURE;
            } else {
                printf(SEPERATOR1 "\n  Load from file successful!\n");
                fclose(fptr);
            }
        }

    } else {
        printf(SEPERATOR1 "\n  Would you like to load courses from file? (Y/n): ");

        fgets(menu_buf, sizeof(menu_buf), stdin);
        if (ui_handle_long_input(menu_buf))
            load_file = false;
        menu_buf[strcspn(menu_buf, "\n")] = '\0';
        menu_buf[0] = (char)tolower((unsigned char)menu_buf[0]);

        if (load_file && menu_buf[0] != 'n') {
            // File parsing loop
            do {
                printf(SEPERATOR1 "\n  Please enter the name of your file (.txt): ");

                fgets(filename, sizeof(filename), stdin);
                if (ui_handle_long_input(filename))
                    break;
                filename[strcspn(filename, "\n")] = '\0';

                const char *ext = strrchr(filename, '.');

                if (!ext || strcmp(ext, ".txt") != 0) {
                    ui_print_error(UI_ERR_FILE_TYPE);
                    break;
                }

                FILE *fptr = fopen(filename, "r");

                // Locate file in correct directory
                if (!fptr) {
                    // Try from data / relative to executable
                    char exec_path[PATH_MAX];
                    if (realpath(argv[0], exec_path)) {
                        char *slash = strrchr(exec_path, '/');
                        if (slash) {
                            *slash = '\0';  // strip executable name
                        }

                        char pathbuf[PATH_MAX];
                        snprintf(pathbuf, sizeof(pathbuf), "%s/../data/%s", exec_path, filename);

                        fptr = fopen(pathbuf, "r");
                    }
                }

                if (!fptr) {
                    ui_print_error(UI_ERR_FILE_NOT_FOUND);
                    break;
                }

                if (!load_from_file(&courses, fptr)) {
                    ui_print_error(UI_ERR_OOM);
                    teardown(&courses);
                    return EXIT_FAILURE;
                } else {
                    printf(SEPERATOR1 "\n  Load from file successful!\n");
                    fclose(fptr);
                }

            } while (0);
        }
    }
    // Menu loop
    while (true) {
        print_menu();

        if (!fgets(menu_buf, sizeof(menu_buf), stdin))
            break;  // Get choice (1-MENU_COUNT)
        if (ui_handle_long_input(menu_buf))
            break;
        menu_buf[strcspn(menu_buf, "\n")] = '\0';

        char *m_endptr;
        long menu_choice = strtol(menu_buf, &m_endptr, 10);

        if (*m_endptr != '\0') {
            ui_print_error(UI_ERR_INVALID_MENU);
            continue;
        }

        if (menu_choice <= MENU_INVALID || menu_choice >= MENU_COUNT) {
            ui_print_error(UI_ERR_INVALID_MENU);
            continue;
        }

        switch (menu_choice) {
            case (MENU_ADD):
                // Course Code
                do {
                    printf(SEPERATOR1 "  Please enter your course code (Ex. SYSC2006): ");

                    if (!fgets(course_code_buf, sizeof(course_code_buf), stdin))
                        break;
                    if (ui_handle_long_input(course_code_buf))
                        break;
                    course_code_buf[strcspn(course_code_buf, "\n")] = '\0';

                    if (!validate_course_code(course_code_buf)) {
                        ui_print_error(UI_ERR_INVALID_CODE);
                        break;
                    }

                    if (check_courses(&courses, course_code_buf)) {
                        ui_print_error(UI_ERR_DUPLICATE);
                        break;
                    }

                    // Course Weight
                    printf(SEPERATOR2 "  Please enter your course weight (1.00, 0.50, 0.25): ");

                    if (!fgets(course_weight_buf, sizeof course_weight_buf, stdin))
                        break;
                    if (ui_handle_long_input(course_weight_buf))
                        break;
                    course_weight_buf[strcspn(course_weight_buf, "\n")] = '\0';

                    char *cw_endptr;
                    course_weight = strtof(course_weight_buf, &cw_endptr);

                    if (*cw_endptr != '\0' || !(course_weight == 1.0f || course_weight == 0.5f ||
                                                course_weight == 0.25f)) {
                        ui_print_error(UI_ERR_INVALID_WEIGHT);
                        break;
                    }

                    // Letter Grade
                    printf(SEPERATOR2
                           "  Please enter your letter grade ((A, B, C, "
                           "D)+/- or F): ");

                    if (!fgets(letter_grade_buf, sizeof letter_grade_buf, stdin))
                        break;
                    if (ui_handle_long_input(letter_grade_buf))
                        break;
                    letter_grade_buf[strcspn(letter_grade_buf, "\n")] = '\0';
                    letter_grade_buf[0] = (char)toupper((unsigned char)letter_grade_buf[0]);

                    if (!validate_letter_grade(letter_grade_buf)) {
                        ui_print_error(UI_ERR_INVALID_GRADE);
                        break;
                    }

                    if (!add_course(&courses, course_code_buf, course_weight, letter_grade_buf)) {
                        ui_print_error(UI_ERR_OOM);
                        teardown(&courses);
                        return EXIT_FAILURE;
                    } else {
                        printf(SEPERATOR2
                               "\n  Course Successfully Added!\n"
                               "  -Course Code: %s\n"
                               "  -Course Weight: %4.2f\n"
                               "  -Letter Grade: %s\n",
                               course_code_buf, course_weight, letter_grade_buf);
                    }
                } while (0);
                break;
            case (MENU_DELETE):
                if (courses.size == 0) {
                    ui_print_error(UI_ERR_EMPTY);
                    continue;
                }

                printf(SEPERATOR1
                       "  Please enter the course code for the course you "
                       "want to delete (Ex. SYSC2006): ");

                if (!fgets(course_code_buf, sizeof(course_code_buf), stdin))
                    break;
                if (ui_handle_long_input(course_code_buf))
                    break;
                course_code_buf[strcspn(course_code_buf, "\n")] = '\0';

                if (!validate_course_code(course_code_buf)) {
                    ui_print_error(UI_ERR_INVALID_CODE);
                } else {
                    if (!check_courses(&courses, course_code_buf)) {
                        ui_print_error(UI_ERR_NOT_FOUND);
                    } else {
                        delete_course(&courses, course_code_buf);
                        printf(SEPERATOR2 "\n  Course successfully deleted\n");
                    }
                }
                break;
            case (MENU_EDIT):
                do {
                    // Get Old course code
                    printf(SEPERATOR1
                           "  Please enter the course code for the course you "
                           "want to edit (Ex. SYSC2006): ");

                    if (!fgets(course_code_buf, sizeof(course_code_buf), stdin))
                        break;
                    if (ui_handle_long_input(course_code_buf))
                        break;
                    course_code_buf[strcspn(course_code_buf, "\n")] = '\0';

                    if (!validate_course_code(course_code_buf)) {
                        ui_print_error(UI_ERR_INVALID_CODE);
                        break;
                    }

                    if (!check_courses(&courses, course_code_buf)) {
                        ui_print_error(UI_ERR_NOT_FOUND);
                        break;
                    }

                    char course_code_old[COURSE_CODE_BUF_LEN];
                    strcpy(course_code_old, course_code_buf);

                    coursenode_t *fetched_node = fetch_node(&courses, course_code_buf);
                    float old_weight = fetched_node->course_weight;
                    char old_grade[LETTER_GRADE_BUF_LEN];
                    strcpy(old_grade, fetched_node->letter_grade);

                    // Get new course code
                    printf(SEPERATOR2 "  Enter new course code (leave blank for no changes): ");

                    if (!fgets(course_code_buf, sizeof(course_code_buf), stdin))
                        break;
                    if (ui_handle_long_input(course_code_buf))
                        break;
                    course_code_buf[strcspn(course_code_buf, "\n")] = '\0';

                    char course_code_new[COURSE_CODE_BUF_LEN];
                    if (course_code_buf[0] == '\0') {
                        strcpy(course_code_new, course_code_old);
                    } else {
                        if (!validate_course_code(course_code_buf)) {
                            ui_print_error(UI_ERR_INVALID_CODE);
                            break;
                        }
                        strcpy(course_code_new, course_code_buf);
                    }

                    if (strcmp(course_code_new, course_code_old) != 0 &&
                        check_courses(&courses, course_code_new)) {
                        ui_print_error(UI_ERR_DUPLICATE);
                        break;
                    }

                    // Get course weight
                    printf(SEPERATOR2 "  Enter new course weight (leave blank for no changes): ");

                    if (!fgets(course_weight_buf, sizeof(course_weight_buf), stdin))
                        break;
                    if (ui_handle_long_input(course_weight_buf))
                        break;
                    course_weight_buf[strcspn(course_weight_buf, "\n")] = '\0';

                    float course_weight_new = old_weight;

                    if (course_weight_buf[0] != '\0') {
                        char *cwn_endptr;
                        float tmp = strtof(course_weight_buf, &cwn_endptr);

                        if (*cwn_endptr != '\0' || !(tmp == 1.0f || tmp == 0.5f || tmp == 0.25f)) {
                            ui_print_error(UI_ERR_INVALID_WEIGHT);
                            break;
                        }

                        course_weight_new = tmp;
                    }

                    printf(SEPERATOR2 "  Enter new letter grade (leave blank for no changes): ");

                    if (!fgets(letter_grade_buf, sizeof(letter_grade_buf), stdin))
                        break;
                    if (ui_handle_long_input(letter_grade_buf))
                        break;
                    letter_grade_buf[strcspn(letter_grade_buf, "\n")] = '\0';

                    char letter_grade_new[LETTER_GRADE_BUF_LEN];
                    if (letter_grade_buf[0] == '\0') {
                        strcpy(letter_grade_new, old_grade);
                    } else {
                        letter_grade_buf[0] = (char)toupper((unsigned char)letter_grade_buf[0]);

                        if (!validate_letter_grade(letter_grade_buf)) {
                            ui_print_error(UI_ERR_INVALID_GRADE);
                            break;
                        }
                        strcpy(letter_grade_new, letter_grade_buf);
                    }

                    // Finalize edit
                    if (!edit_course(&courses, course_code_old, course_code_new, course_weight_new,
                                     letter_grade_new)) {
                        ui_print_error(UI_ERR_OOM);
                        teardown(&courses);
                        return EXIT_FAILURE;
                    }
                    printf(SEPERATOR2
                           "\n  Course Successfully Edited!\n"
                           "  -Old Course Code: %s\n"
                           "  -New Course Code: %s\n"
                           "  -Course Weight: %4.2f\n"
                           "  -Letter Grade: %s\n",
                           course_code_old, course_code_new, course_weight_new, letter_grade_new);
                } while (0);
                break;
            case (MENU_DISPLAY):
                display_grades(&courses);
                break;
            case (MENU_EXIT):
                printf("\n  Goodbye!\n");
                teardown(&courses);
                return EXIT_SUCCESS;
                break;
        }
    }
}