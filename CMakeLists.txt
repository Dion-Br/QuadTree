cmake_minimum_required(VERSION 3.20)
project(QuadTreeProject)

set(CMAKE_CXX_STANDARD 20)

# 1. Define the library (Header-only)
add_library(quadtree_lib INTERFACE)
target_include_directories(quadtree_lib INTERFACE src)
target_sources(quadtree_lib INTERFACE src/Quadtree.hpp)

if(MSVC)
    target_compile_options(quadtree_lib INTERFACE /W4 /WX)
else()
    target_compile_options(quadtree_lib INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

# 2. Main executable (demo)
add_executable(quadtree src/main.cpp)
target_link_libraries(quadtree PRIVATE quadtree_lib)

# 3. Test executable (SEPARATE from quadtree)
add_executable(quadtree_test quadtree_test.cpp)
target_link_libraries(quadtree_test PRIVATE quadtree_lib)