message("toolchain: ${CMAKE_TOOLCHAIN_FILE}")
find_package(spdlog REQUIRED)

add_library(spdlog INTERFACE)
target_link_libraries(spdlog INTERFACE spdlog::spdlog)

if (TARGET spdlog)
    message("spdlog found")
else()
    message("spdlog not found")
endif()