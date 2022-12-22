add_subdirectory(cglm)
add_library(cglm_lib INTERFACE)
target_link_libraries(cglm_lib INTERFACE cglm)
target_link_libraries(cglm_lib INTERFACE m)
add_subdirectory(stb)
add_subdirectory(log)
add_subdirectory(pcg_basic)
add_subdirectory(sorting)
add_subdirectory(libyaml)

add_subdirectory(yaml-cpp)

add_library(libsimdpp INTERFACE)
target_include_directories(libsimdpp INTERFACE "libsimdpp/")
