

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "../")
target_link_libraries(stb INTERFACE m)