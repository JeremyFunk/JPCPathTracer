
enable_language(CXX)
add_library(sorting STATIC "sort.h" "sort.cpp")

target_include_directories(sorting PUBLIC "../")

set_target_properties(sorting PROPERTIES LINKER_LANGUAGE CXX)
