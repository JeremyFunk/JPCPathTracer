cmake_minimum_required(VERSION 3.10)

project(jcp_pathtracer LANGUAGES C)
set(CMAKE_C_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# include(cmake/macros.cmake) benutze ich noch nicht
add_subdirectory(libs)
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

set(AUTOSRC
"test.c"
"src/renderer.h"
"src/bvh.h"
"src/lights.h"
"src/camera.h"
"src/spherical_coords.h"
"src/utils.h"
"src/bsdf.h"
"src/sampling.h"
"src/types.h"
"src/texture.h"
"src/shaders.h"
"src/integrator.h"
"src/allocators.h"
"src/sampler.h"
"src/bsdf.c"
"src/shaders.c"
"src/renderer.c"
"src/integrator.c"
"src/bvh.c"
"src/sampling.c"
"src/allocators.c"
"src/texture.c"
"src/sampler.c"
"src/lbvh.c"
"src/lights.c"
"src/camera.c"
"src/shaders/shader2.c"
"src/shaders/shader_diffuse.c"
"src/bsdfs/diffuse.c"
"include/jpc_api.h"
)

set(LIBS
cglm_lib
stb
log
pcg_basic
m
sorting
)

add_library(tracer SHARED "${AUTOSRC}")
target_include_directories(tracer PUBLIC "include")
target_link_libraries(tracer ${LIBS} )
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(tracer PRIVATE "-fms-extensions")

endif()
add_executable(ctest "test.c")
target_link_libraries(ctest tracer)
