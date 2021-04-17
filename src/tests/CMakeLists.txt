set(AUTOSRC
"test_utils.h"
"acceleration/LBVHTest.cpp"
"acceleration/SortTest.cpp"
"acceleration/testing.cpp"
"cameras/ProjectionCamera.cpp"
"math/MatTest.cpp"
"math/SpectrumTest.cpp"
"math/VecTest.cpp"
"raytracing_core/ShadingPrograms.cpp"
"raytracing_core/TriangleIntersection.cpp"
"samplers/GridSamplers.cpp"
"shaders/BsdfFactory.cpp"
"shaders/DiscreteSampler.cpp"
"shaders/ShaderFunction.cpp"
"utilities/imageio.cpp"
)
set(INC
"../")

set(LIBS 
    jpc_tracer
)

jpctr_add_gtest(jpctracer_tests "${AUTOSRC}" "${INC}" "${LIBS}" )