

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "../")
if(${clang_gnu})
	target_link_libraries(stb INTERFACE m)
endif()