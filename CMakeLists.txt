# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name
project(Fserver)

# Set the output folder where your program will be created

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})


# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

set(SOURCES_DIR ${PROJECT_SOURCE_DIR}/src)

message("infor path: \n\")
message("- PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR} \n\")
message("- CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR} \n\")
message("- EXECUTABLE_OUTPUT_PATH: ${EXECUTABLE_OUTPUT_PATH} \n\")
message("- LIBRARY_OUTPUT_PATH: ${LIBRARY_OUTPUT_PATH} \n\")

add_compile_options(
    -std=c++11
#        -D_DEBUG
#        -g
)

add_executable(Fserver ${SOURCES_DIR}/main.cpp)
