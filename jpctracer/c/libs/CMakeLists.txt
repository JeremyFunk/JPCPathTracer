set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(cglm)
add_library(cglm_lib INTERFACE)
target_link_libraries(cglm_lib INTERFACE cglm)
if(${clang_gnu})
	target_link_libraries(cglm_lib INTERFACE m)
endif()

add_subdirectory(stb)
add_subdirectory(log)
add_subdirectory(pcg_basic)
add_subdirectory(sorting)
add_subdirectory(libyaml)

add_subdirectory(yaml-cpp)

add_library(libsimdpp INTERFACE)
target_include_directories(libsimdpp INTERFACE "libsimdpp/")

#add_library(ufbx_lib STATIC "./ufbx/ufbx.c")
#target_include_directories(ufbx_lib PUBLIC ".")