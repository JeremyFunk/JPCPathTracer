

set(clang_gnu ${CMAKE_CXX_COMPILER_ID} MATCHES "clang|GNU" OR ${CMAKE_C_COMPILER_ID} MATCHES "clang|GNU" )
# include(cmake/macros.cmake) benutze ich noch nicht
add_subdirectory(libs)
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(&{clang_gnu})
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
  message("GNU or Clang compiler!")
endif()

set(AUTOSRC
"test.c"
"src/bsdf.cpp"
"src/geom_log.cpp"
"src/renderer.h"
"src/config.h"
"src/lights.h"
"src/camera.h"
"src/scattering.h"
"src/spherical_coords.h"
"src/utils.h"
"src/bsdf.h"
"src/compiler_builtins.h"
"src/sampling.h"
"src/types.h"
"src/texture.h"
"src/maths.h"
"src/shaders.h"
"src/integrator.h"
"src/iterator.h"
"src/geom_log.h"
"src/allocators.h"
"src/sampler.h"
"src/algorithms.h"
"src/shaders.c"
"src/renderer.c"
"src/integrator.c"
"src/sampling.c"
"src/scattering.c"
"src/allocators.c"
"src/texture.c"
"src/sampler.c"
"src/lights.c"
"src/camera.c"
"src/bsdf.hpp"
"src/allocators.hpp"
"src/shaders/shader_diffuse.c"
"src/bvh/bvh.h"
"src/bvh/bvh_ref.h"
"src/bvh/bounds3d.h"
"src/bvh/shapes.h"
"src/bvh/bvh_builder.h"
"src/bvh/lbvh.h"
"src/bvh/traverse.h"
"src/bvh/boundsN.h"
"src/bvh/shapes.c"
"src/bvh/bvh_builder.c"
"src/bvh/bvh.c"
"src/bvh/lbvh.c"
"src/bvh/naive.c"
"src/bvh/traverse.c"
"src/simd/simd-2.h"
"src/simd/simd.h"
"src/simd/simd-8.h"
"src/simd/simd-4.h"
"src/bsdfs/diffuse.cpp"
"include/jpc_api.h"
)

set(LIBS
cglm_lib
stb
log
pcg_basic
m
sorting
libsimdpp
)

add_library(tracer SHARED "${AUTOSRC}")
target_include_directories(tracer PUBLIC "include")
target_link_libraries(tracer ${LIBS} )
if(${clang_gnu})
  target_compile_options(tracer PRIVATE "-fms-extensions")

  target_compile_options(tracer PRIVATE "-march=native")

endif()

set_property(TARGET tracer PROPERTY C_STANDARD 11)

add_executable(ctest "test.c")
target_link_libraries(ctest tracer)
all_warnings(tracer)
