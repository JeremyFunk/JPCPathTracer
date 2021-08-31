add_library(log STATIC "log.c" "log.h")
target_include_directories(log PUBLIC "../")

if(MSVC)
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(log PUBLIC "-fPIC")
endif()