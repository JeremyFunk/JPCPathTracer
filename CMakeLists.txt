
cmake_minimum_required(VERSION 3.10)

project(jcp_pathtracer LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

macro(all_warnings TARGET_NAME)
    if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
        if(MSVC)
            target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
        else()
            target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wpedantic -Wextra -Werror )
        endif()
    endif()
endmacro()

add_subdirectory(jpctracer)
