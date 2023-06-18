

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


add_executable(integrator_test "integrator.c")
target_link_libraries(integrator_test tracer)

# target_compile_options(integrator_test PUBLIC "/fsanitize=address /Zi")

target_include_directories(integrator_test PRIVATE "../c/src/")


add_executable(dataformat_test "dataformat.c")
target_link_libraries(dataformat_test tracer cglm_lib)


add_subdirectory(test_scenes)