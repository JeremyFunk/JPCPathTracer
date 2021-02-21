find_package(range-v3 CONFIG REQUIRED)
add_library(rangev3 INTERFACE)
# Note: 2 target(s) were omitted.
target_link_libraries(rangev3 INTERFACE range-v3 range-v3-meta range-v3::meta range-v3-concepts)
