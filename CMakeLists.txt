cmake_minimum_required(VERSION 3.21)
project(ray_maze C)


# Dependencies
set(CMAKE_C_STANDARD 11) # Requires C11 standard
add_subdirectory(raylib)

include_directories(include)

# All warnings/errors for the code we wrote:
if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Wmissing-braces)
    # TODO: Actually fix all the problems ASAN finds…

    if ($ENV{USE_ASAN})
        # Address Sanitizer
        add_compile_options(-O1 -g -fsanitize=address)
        add_link_options(-g -fsanitize=address)
    endif()
endif()

add_library(stack stack.c)
add_library(queue queue.c)
add_library(maze_gen maze.c)
add_library(export export.c)
add_library(import import.c)
add_executable(ray_maze main.c)

target_link_libraries(ray_maze raylib maze_gen stack queue export import)


# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(ray_maze "-framework IOKit")
    target_link_libraries(ray_maze "-framework Cocoa")
    target_link_libraries(ray_maze "-framework OpenGL")
endif()
