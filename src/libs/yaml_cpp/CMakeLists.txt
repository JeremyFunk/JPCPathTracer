find_package(yaml-cpp CONFIG REQUIRED)
add_library(yaml_cpp INTERFACE)

target_link_libraries(yaml_cpp INTERFACE yaml-cpp)

if (TARGET yaml_cpp)
    message("yaml_cpp found")
else()
    message("yaml_cpp not found")
endif()
