cmake_minimum_required(VERSION 3.20)
project(QuadTreeProject)

set(CMAKE_CXX_STANDARD 20)

# 1. Define the library (Header-only)
add_library(quadtree_lib INTERFACE)
target_include_directories(quadtree_lib INTERFACE src)
target_sources(quadtree_lib INTERFACE src/Quadtree.hpp)

# This checks which compiler is being used and applies the correct flags.
if(MSVC)
    # Flags for Visual Studio
    target_compile_options(quadtree_lib INTERFACE /W4 /WX)
else()
    # Flags for GCC and Clang
    target_compile_options(quadtree_lib INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

# 2. Define the executable with the specific name 'quadtree'
add_executable(quadtree src/main.cpp
        src/AxisAlignedBoundingBox.hpp)

# 3. Link them together
target_link_libraries(quadtree PRIVATE quadtree_lib)