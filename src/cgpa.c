/***********************************
 * cgpa.c
 *
 * CGPA Calculator - core calculation and list management logic
 *
 * Handles:
 * - linked list integration for courses
 * - grade-to-credit conversion
 * - CGPA aggregation
 *
 * Author: Arul Rao (Vidonicle)
 * License: MIT
 ***********************************/
#define _POSIX_C_SOURCE 200809L

#include "cgpa.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "1.4.0"

static const grade_map_t grade_map[] = {{"A+", 12.0f}, {"A", 11.0f}, {"A-", 10.0f}, {"B+", 9.0f},
                                        {"B", 8.0f},   {"B-", 7.0f}, {"C+", 6.0f},  {"C", 5.0f},
                                        {"C-", 4.0f},  {"D+", 3.0f}, {"D", 2.0f},   {"D-", 1.0f},
                                        {"F", 0.0f}};
static const size_t GRADE_MAP_LEN = sizeof grade_map / sizeof grade_map[0];

// Print main menu helper to print on loop
void print_menu(void) {
    printf(
        "\n =============WELCOME===============\n"
        "  -  Welcome to the CGPA Calculator\n"
        "  -  Please follow all instructions\n"
        "  -  Currently only compatible with the system\n"
        "     used by Carleton University\n"
        "  -  Version %s\n"
        " =========CGPA CALCULATOR===========",
        VERSION);

    printf(
        "\n\n  1. Add a course\n"
        "  2. Delete a course\n"
        "  3. Edit a course\n"
        "  %d. Display Courses and CGPA\n"
        "  %d. Exit\n\n"
        "  Enter your selection (1-%d): ",
        MENU_DISPLAY, MENU_EXIT, MENU_EXIT);
}

void init_list(list_courses_t *list) {
    memset(&(list->sentinel), 0, sizeof(list->sentinel));

    list->sentinel.next = &(list->sentinel);
    list->sentinel.prev = &(list->sentinel);

    list->size = 0;
}

// Add course in alphanumerical order
bool add_course(list_courses_t *courses, const char *course_code, float course_weight,
                const char *letter_grade) {
    coursenode_t *new_node = malloc(sizeof(coursenode_t));
    if (!new_node)
        return false;

    strcpy(new_node->course_code, course_code);
    new_node->course_weight = course_weight;
    strcpy(new_node->letter_grade, letter_grade);
    new_node->credits_earned = earned_credits(course_weight, letter_grade);
    new_node->next = NULL;
    new_node->prev = NULL;

    coursenode_t *curr = courses->sentinel.next;

    while (curr != &(courses->sentinel) && strcmp(curr->course_code, course_code) < 0) {
        curr = curr->next;
    }

    new_node->next = curr;
    new_node->prev = curr->prev;

    curr->prev->next = new_node;
    curr->prev = new_node;

    courses->size++;

    return true;
}

// Delete node given by course code
void delete_course(list_courses_t *courses, const char *course_code) {
    coursenode_t *to_delete = fetch_node(courses, course_code);
    if (!to_delete)
        return;

    to_delete->prev->next = to_delete->next;
    to_delete->next->prev = to_delete->prev;

    free(to_delete);

    courses->size--;
}

bool edit_course(list_courses_t *courses, const char *course_code_old, const char *course_code_new,
                 float course_weight_new, const char *letter_grade_new) {
    delete_course(courses, course_code_old);
    return add_course(courses, course_code_new, course_weight_new, letter_grade_new);
}

// Load courses from file
bool load_from_file(list_courses_t *courses, FILE *fptr) {
    char line[256];

    // Loop through each line of file
    while (fgets(line, sizeof(line), fptr)) {
        if (line[0] == '\n' || line[0] == '#')
            continue;  // Skip comments

        // Default values
        char course_code[COURSE_CODE_BUF_LEN] = {0};
        float course_weight = 0.0f;
        char letter_grade[LETTER_GRADE_BUF_LEN] = "F";

        char *tok_entptr;
        char *tok = strtok_r(line, " \t\n,", &tok_entptr);

        // Skip if course code is not present
        if (!tok)
            continue;
        if (!validate_course_code(tok))
            continue;

        strcpy(course_code, tok);

        // Get other fields for course
        while ((tok = strtok_r(NULL, " \t\n,", &tok_entptr)) != NULL) {
            // Get course weight
            char *w_endptr;
            float weight = strtof(tok, &w_endptr);
            if (w_endptr != tok && *w_endptr == '\0') {
                course_weight = weight;
                continue;
            }

            // Get letter grade
            if (validate_letter_grade(tok)) {
                strcpy(letter_grade, tok);
                continue;
            }
        }

        if (!add_course(courses, course_code, course_weight, letter_grade))
            return false;  // Exits on malloc failure
    }

    return true;
}

// Fetch course from list
coursenode_t *fetch_node(list_courses_t *courses, const char *course_code) {
    coursenode_t *curr = courses->sentinel.next;

    while (curr != &(courses->sentinel)) {
        if (strcmp(curr->course_code, course_code) == 0) {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

// Calculate earned credits depending on course weight and letter grade
float earned_credits(float course_weight, const char *letter_grade) {
    for (size_t i = 0; i < GRADE_MAP_LEN; i++) {
        if (strcmp(letter_grade, grade_map[i].grade) == 0)
            return (grade_map[i].value * course_weight);
    }

    return 0.0f;
}

// Print grades and formats column sections
void display_grades(list_courses_t *courses) {
    float accum_credits = 0.0f;
    float accum_weight = 0.0f;

    printf(SEPERATOR1 "  Course Code  Weight  Grade  Credits\n");
    printf(" -------------------------------------\n");

    int i = 1;
    for (coursenode_t *curr = courses->sentinel.next; curr != &(courses->sentinel);
         curr = curr->next) {
        int grade_w = (curr->letter_grade[1] == '\0') ? 4 : 5;
        int credit_w = (curr->letter_grade[1] == '\0') ? 9 : 8;

        printf("  %-12s %4.2f %*s %*.2f\n", curr->course_code, curr->course_weight, grade_w,
               curr->letter_grade, credit_w, curr->credits_earned);

        accum_credits += curr->credits_earned;
        accum_weight += curr->course_weight;
        i++;
    }

    float cgpa = accum_weight > 0 ? accum_credits / accum_weight : 0.0f;
    float gpa = cgpa > 1 ? (cgpa + 1) / 3 : 2 * cgpa / 3;

    printf(
        "\n\n  Total Credits Earned: %4.2f"
        "\n  Total Credits Completed: %4.2f"
        "\n"
        "\n  Current CGPA (12.00 scale): %4.2f"
        "\n  Estimated GPA (4.00 scale): %4.2f\n" SEPERATOR2,
        accum_credits, accum_weight, cgpa, gpa);
}

// Check for existing courses
bool check_courses(list_courses_t *courses, const char *course_code) {
    coursenode_t *curr = courses->sentinel.next;

    while (curr != &(courses->sentinel)) {
        if (strcmp(curr->course_code, course_code) == 0)
            return true;

        curr = curr->next;
    }

    return false;
}

// Check validation for course code
bool validate_course_code(char *course_code) {
    if (!(strlen(course_code) == 8)) {
        return false;
    }

    for (size_t i = 0; i < 4; i++) {
        if (!isalpha(course_code[i]))
            return false;

        course_code[i] = (char)toupper((unsigned char)course_code[i]);
    }

    for (size_t i = 4; i < 8; i++) {
        if (!isdigit(course_code[i]))
            return false;
    }

    return true;
}

// Check validation for letter grade
bool validate_letter_grade(const char *letter_grade) {
    for (size_t i = 0; i < GRADE_MAP_LEN; i++) {
        if (strcmp(letter_grade, grade_map[i].grade) == 0)
            return true;
    }

    return false;
}

// Deconstruct and free list
void teardown(list_courses_t *courses) {
    coursenode_t *curr = courses->sentinel.next;
    while (curr != &(courses->sentinel)) {
        coursenode_t *next = curr->next;
        free(curr);
        curr = next;
    }
}

// Flush input buffer
void flush_stdin(void) {
    int c_flush;
    while ((c_flush = getchar()) != '\n' && c_flush != EOF);
}
