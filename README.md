# cgpa-calculator

A simple command-line CGPA calculator written in C.

Calculates CGPA based on course letter grades and course weights using the
Carleton University grading scale.

## Features

- Add courses with code, weight, and letter grade
- Automatically sorts courses by course code
- Displays total credits earned, credits completed, and CGPA
- Input validation and error handling
- Linked-list based storage
- Sentinel node to keep addition and traversal easier

## Build and Usage

Requires a C compiler (GCC or Clang) and CMake.

```bash
mkdir build
cd build
cmake ..
make
```

From the project root, execute:

```bash
./build/cgpa
```

or:

```bash
./build/cgpa ./data/courses
```

to directly load a file

## Loading from file

The program automatically searches the current directory and then `data/`.

To use this feature, place course files in the `data/` directory.

Courses can also be entered manually through the interactive menu.

Files must follow this format:

```txt
SYSC2006 0.5 A+
CCDP2100 0.5 A
MATH1005 0.5 A-
```

## Notes

- Intended for personal and educational use
- Grading scale is hard-coded for Carleton University
- This project is not affiliated with Carleton University in any way
- Built as a learning project to practice C, memory management, and CLI design


## Lessons Learned

What did you learn while building this project? What challenges did you face and how did you overcome them?

- Robust input handling in C requires explicit flushing and careful use of `fgets`.
- The `!fgets(...)` pattern is useful for detecting EOF and input errors cleanly.
- `printf` formatting is more powerful (and more subtle) than expected when aligning CLI output.
- Building a non-trivial program reinforced existing knowledge of structs, pointers, and dynamic memory management.
