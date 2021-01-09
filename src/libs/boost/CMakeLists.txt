
find_package(Boost COMPONENTS program_options  system unit_test_framework)

add_library(boost INTERFACE)
target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})
target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})