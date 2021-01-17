cmake_minimum_required(VERSION 3.3)
project(c_bsdfs)
add_definitions("-Wall" "-g")
include_directories(${PROJECT_SOURCE_DIR})
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)

set(SRC_FILES "main.cpp" "bsdfparams.h")

add_executable(c_bsdfs ${SRC_FILES})