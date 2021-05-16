set(AUTOSRC
"test_utils.h"
"shaders/DiscreteSampler.cpp"
"shaders/GlossyBsdf.cpp"
"shaders/BsdfFactory.cpp"
"shaders/ShaderFunction.cpp"
"acceleration/LBVHTest.cpp"
"acceleration/BoundsTest.cpp"
"acceleration/SortTest.cpp"
"app/seralization.cpp"
"raytracing_core/TriangleIntersection.cpp"
"raytracing_core/ShadingPrograms.cpp"
"raytracing_core/SphereIntersection.cpp"
"samplers/GridSamplers.cpp"
"utilities/imageio.cpp"
"math/VecTest.cpp"
"math/MatTest.cpp"
"math/SpectrumTest.cpp"
"cameras/ProjectionCamera.cpp"
)
set(INC
"../")

set(LIBS 
    jpc_tracer
    jpctracer_app_lib
)

jpctr_add_gtest(jpctracer_tests "${AUTOSRC}" "${INC}" "${LIBS}" )