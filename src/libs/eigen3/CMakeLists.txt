find_package(Eigen3 CONFIG REQUIRED)

add_library(eigen3 INTERFACE)
target_link_libraries(eigen3 INTERFACE Eigen3::Eigen)
