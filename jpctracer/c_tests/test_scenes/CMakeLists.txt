
add_executable(test_scene1 "scene1.c")
target_link_libraries(test_scene1 tracer)


add_executable(scene_blue_monkey scene_blue_monkey.c)
target_link_libraries(scene_blue_monkey tracer)