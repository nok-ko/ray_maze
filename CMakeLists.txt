cmake_minimum_required(VERSION 3.21)
project(ray_maze C)

# Dependencies
find_package(raylib 4.0.0 QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            URL https://github.com/raysan5/raylib/archive/refs/tags/4.0.0.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

set(CMAKE_C_STANDARD 11) # Requires C11 standard

add_library(stack stack.c)
add_library(maze_gen maze.c)
add_executable(ray_maze main.c)

target_link_libraries(ray_maze raylib maze_gen stack)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(ray_maze "-framework IOKit")
    target_link_libraries(ray_maze "-framework Cocoa")
    target_link_libraries(ray_maze "-framework OpenGL")
endif()
