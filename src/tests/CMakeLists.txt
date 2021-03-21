set(AUTOSRC
"test_utils.h"
"shaders/mixer.cpp"
"shaders/mirrorbsdf.cpp"
"shaders/shader_concepts.cpp"
"raytracing_core/TriangleIntersection.cpp"
"raytracing_core/ShadingPrograms.cpp"
"samplers/GridSamplers.cpp"
"math/VecTest.cpp"
"cameras/ProjectionCamera.cpp"
)
set(INC
"")

set(LIBS 
    jpc_tracer
)

jpctr_add_gtest(jpctracer_tests "${AUTOSRC}" "${INC}" "${LIBS}" )
