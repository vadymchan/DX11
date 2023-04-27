cmake_minimum_required(VERSION 3.24)
project(project)

set(SOLUTION_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(engine)
add_subdirectory(project)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT project)