
set(SRC 
    "geometry.hpp"
    "random_geometry.hpp"
    "random_geometry.cpp"
    "handle_vector.hpp"
    "algorithms.hpp")

add_library(tracercpp STATIC "${SRC}")
target_include_directories(tracercpp PUBLIC "./")
target_link_libraries(tracercpp PUBLIC tracer)



add_executable(test_scene "test_scene.cpp")
#target_link_libraries(test_scene tracer)
target_link_libraries(test_scene tracercpp)

set_property(TARGET test_scene PROPERTY CXX_STANDARD 20)


