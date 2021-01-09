
find_package(OpenImageIO REQUIRED)

add_library(openimageio INTERFACE)
target_link_libraries(openimageio INTERFACE OpenImageIO::OpenImageIO)

