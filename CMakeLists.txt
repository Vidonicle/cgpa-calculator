cmake_minimum_required(VERSION 3.10)
project(cgpa_calculator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(cgpa
    src/main.c
    src/cgpa.c
    src/ui_errors.c
)

target_include_directories(cgpa PRIVATE include)

target_compile_definitions(cgpa PRIVATE
    _XOPEN_SOURCE=700
)

if(MSVC)
    target_compile_options(cgpa PRIVATE /W4)
else()
    target_compile_options(cgpa PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()