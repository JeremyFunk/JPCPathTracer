set(clang_gnu ${CMAKE_CXX_COMPILER_ID} MATCHES "clang|GNU" OR ${CMAKE_C_COMPILER_ID} MATCHES "clang|GNU" )

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_subdirectory(c)
add_subdirectory(cpp)

add_subdirectory(c_tests)
