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

#define COURSE_CODE_BUF_LEN 10   // 8 chars max + '\n' + '\0'
#define COURSE_WEIGHT_BUF_LEN 6  // 4 chars max + '\n' + '\0'
#define LETTER_GRADE_BUF_LEN 5   // 3 chars max + '\n' + '\0'

#define MENU_BUF_LEN 64            // 62 chars + '\n' + '\0'
#define FILENAME_LEN MENU_BUF_LEN  // Length of filename

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
} course_list_t;

typedef struct {
    char course_code_buf[COURSE_CODE_BUF_LEN];
    char course_weight_buf[COURSE_WEIGHT_BUF_LEN];
    char letter_grade_buf[LETTER_GRADE_BUF_LEN];
    char filename[FILENAME_LEN];
    char menu_buf[MENU_BUF_LEN];
} input_buffers_t;

typedef struct {
    input_buffers_t input_buffers;
    course_list_t courses;
} calculator_t;

typedef struct {
    const char *grade;
    float value;
    bool zero_weight;
} grade_map_t;

typedef enum {
    MENU_INVALID = 0,  // Invalid option

    MENU_ADD,
    MENU_EDIT,
    MENU_DELETE,
    MENU_DISPLAY,
    MENU_EXIT,

    MENU_COUNT
} menu_option_t;

/*
 * Prints the main menu
 *
 * Prints the main menu after every
 * interaction between user and program
 * completes.
 */
void print_menu(void);

/*
 * Initializes the course list.
 *
 * Allocates a sentinel node in preparation
 * for the course list.
 */
void initialize_courses(course_list_t *list);

/*
 * Adds a course to the course list.
 *
 * Allocates a new course node and inserts it
 * into the list, sets the next and previous pointers,
 * failure causes program termination, handled externally.
 */
bool add_course(course_list_t *courses, const char *course_code, float course_weight,
                const char *letter_grade);
/*
 * Edits a course present in the list.
 *
 * Deletes the course from the course list then adds a new course
 * Uses add_course(), therefore failure results in termination.
 */

bool edit_course(course_list_t *courses, const char *course_code_old, const char *course_code_new,
                 float course_weight_new, const char *letter_grade_new);

/*
 * Deletes a course from the course list.
 *
 * Detaches course from the course list and frees it.
 */
void delete_course(course_list_t *courses, const char *course_code);

/*
 * Loads courses from file.
 *
 * Loads courses from specified file,
 * file is obtained from argv[1] if argc is 2,
 * or from manual input otherwise
 *
 * Uses add_course, therefore failure results in termination.
 */
bool load_from_file(course_list_t *courses, FILE *fptr);

/*
 * Checks course list to see if a course exists.
 *
 * Searches through the course list and returns true
 * if course exists, false otherwise.
 */
bool check_courses(course_list_t *courses, const char *course_code);

/*
 * Fetches and returns the desired node.
 *
 * Searches through the course list and returns the memory address
 * of the node requested, returns NULL if node can not be
 * located.
 */
coursenode_t *fetch_node(course_list_t *courses, const char *course_code);

/*
 * Calculates the credits earned from a course.
 */
float earned_credits(float course_weight, const char *letter_grade);

/*
 * Displays all courses.
 *
 * Displays courses, weight, earned grade, and credits earned, in a list form.
 * Displays total credits earned and completed, along with CGPA and estimated GPA.
 */
void display_grades(course_list_t *courses);

/*
 * Checks if a course code is valid
 *
 * Checks if the input course code is valid,
 * returns true if so, false otherwise.
 */
bool validate_course_code(char *course_code);

/*
 * Checks if a course weight is valid
 *
 * Checks if the input course weight is valid,
 * returns true if so, false otherwise.
 */
bool validate_course_weight(float course_weight);

/*
 * Checks if a letter grade is valid
 *
 * Checks if the input letter grade is valid,
 * depending on course weight, valid letter grades may vary
 * returns true if so, false otherwise.
 */
bool validate_letter_grade(const char *letter_grade, float course_weight);

/*
 * Tears down the course list
 */
void teardown(course_list_t *course);

#endif /* CGPA_H */