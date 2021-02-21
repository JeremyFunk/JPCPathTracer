cmake_minimum_required(VERSION 3.16)

project(jcp_pathtracer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


include(cmake/macros.cmake)

add_subdirectory(src)

