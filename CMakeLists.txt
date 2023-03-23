cmake_minimum_required(VERSION 3.26)
project(project)

set(SOLUTION_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(engine)
add_subdirectory(project)
