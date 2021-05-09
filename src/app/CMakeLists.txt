set(SRC
"materials.cpp"
"Archive.cpp"
"ParseGeometry.cpp"
"ParseRenderer.cpp"
"main.cpp"
"Archive.h"
"ParseScene.h"
)


set(INC
    "../"
)

set(LIBS
    yaml_cpp
    jpc_tracer
    spdlog
)

jpctr_add_library(jpctracer_app_lib "${SRC}" "${INC}" "${LIBS}" STATIC)


add_executable(jpctracer_app "main.cpp")

target_include_directories(jpctracer_app PUBLIC "${INC}")

target_link_libraries(jpctracer_app jpctracer_app_lib)