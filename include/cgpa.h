/***********************************
 * cgpa.h
 *
 * CGPA Calculator - public interface and shared definitions
 *
 * Defines:
 * - course data structures
 * - grading validation helpers
 * - input-related constants
 *
 * Author: Arul Rao (Vidonicle)
 * License: MIT
 ***********************************/

#ifndef CGPA_H
#define CGPA_H

#include <stdbool.h>
#include <stdio.h>

#define COURSE_CODE_BUF_LEN 10   // 8 chars + '\n' + '\0'
#define COURSE_WEIGHT_BUF_LEN 6  // numeric input + '\n' + '\0'
#define LETTER_GRADE_BUF_LEN 4   // 2 chars + '\n' + '\0'

#define SEPERATOR1 "\n ===================================\n"  // Seperator for UI elements
#define SEPERATOR2 " ===================================\n"    // Seperator for stacked elements

typedef struct course {
    char course_code[COURSE_CODE_BUF_LEN];
    char letter_grade[LETTER_GRADE_BUF_LEN];
    float course_weight;
    float credits_earned;
    struct course *next;
    struct course *prev;
} coursenode_t;

typedef struct {
    coursenode_t sentinel;
    size_t size;
} list_courses_t;

typedef struct {
    const char *grade;
    float value;
} grade_map_t;

typedef enum {
    MENU_INVALID = 0,  // Invalid option

    MENU_ADD,
    MENU_DELETE,
    MENU_EDIT,
    MENU_DISPLAY,
    MENU_EXIT,

    MENU_COUNT
} menu_option_t;

void print_menu(void);

void init_list(list_courses_t *list);

bool add_course(list_courses_t *courses, const char *course_code, float course_weight,
                const char *letter_grade);

void delete_course(list_courses_t *courses, const char *course_code);

bool edit_course(list_courses_t *courses, const char *course_code_old, const char *course_code_new,
                 float course_weight_new, const char *letter_grade_new);

bool load_from_file(list_courses_t *courses, FILE *fptr);

coursenode_t *fetch_node(list_courses_t *courses, const char *course_code);

float earned_credits(float course_weight, const char *letter_grade);

void display_grades(list_courses_t *courses);

bool check_courses(list_courses_t *courses, const char *course_code);

bool validate_course_code(char *course_code);

bool validate_letter_grade(const char *letter_grade);

void teardown(list_courses_t *course);

void flush_stdin(void);

#endif /* CGPA_H */