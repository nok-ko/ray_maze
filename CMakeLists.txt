cmake_minimum_required(VERSION 3.21)
project(ray_maze C)

# Dependencies
set(CMAKE_C_STANDARD 11) # Requires C11 standard
add_subdirectory(raylib)

include_directories(include)
#add_library(raygui raygui.c)
add_library(stack stack.c)
add_library(queue queue.c)
add_library(maze_gen maze.c)
add_library(export export.c)
add_executable(ray_maze main.c)

target_link_libraries(ray_maze raylib maze_gen stack queue export)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(ray_maze "-framework IOKit")
    target_link_libraries(ray_maze "-framework Cocoa")
    target_link_libraries(ray_maze "-framework OpenGL")
endif()
