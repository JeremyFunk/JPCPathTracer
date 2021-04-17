set(AUTOSRC
"test_utils.h"
"shaders/DiscreteSampler.cpp"
"shaders/GlossyBsdf.cpp"
"shaders/BsdfFactory.cpp"
"shaders/ShaderFunction.cpp"
"raytracing_core/TriangleIntersection.cpp"
"raytracing_core/ShadingPrograms.cpp"
"raytracing_core/SphereIntersection.cpp"
"samplers/GridSamplers.cpp"
"utilities/imageio.cpp"
"math/VecTest.cpp"
"math/SpectrumTest.cpp"
"cameras/ProjectionCamera.cpp"
)
set(INC
"")

set(LIBS 
    jpc_tracer
)

jpctr_add_gtest(jpctracer_tests "${AUTOSRC}" "${INC}" "${LIBS}" )
