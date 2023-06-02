

add_executable(intervall_test "intervall_test.cpp")
target_link_libraries(intervall_test tracer)
target_include_directories(intervall_test PRIVATE "../c/src/")


add_executable(bvh_comparision_test "bvh_comparision_test.cpp")
target_link_libraries(bvh_comparision_test tracercpp)
target_include_directories(bvh_comparision_test PRIVATE "../c/src/")

add_executable(bvh_test "bvh_test.cpp")
target_link_libraries(bvh_test tracercpp)
target_include_directories(bvh_test PRIVATE "../c/src/")

add_executable(load_scene_test "load_scene.c")
target_link_libraries(load_scene_test tracer)

target_compile_options(load_scene_test PUBLIC "/fsanitize=address /Zi")