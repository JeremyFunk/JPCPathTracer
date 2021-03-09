set(AUTOSRC
"JPCTracer.h"
"Concepts.h"
"shaders/Cache.cpp"
"shaders/Cache.h"
"shaders/shaders.h"
"shaders/sampling/SphereSampling.cpp"
"shaders/sampling/SphereSampling.h"
"shaders/bsdf/Concepts.h"
"shaders/bsdf/RootBsdf.h"
"shaders/values/Concepts.cpp"
"shaders/values/Mixer.h"
"shaders/values/Concepts.h"
"raytracing/TracingEngine.h"
"raytracing/Scene.h"
"raytracing/Logger.h"
"raytracing/Geometry.h"
"raytracing/Base.h"
"raytracing/SurfaceInteraction.h"
"raytracing/TraceRay.h"
"raytracing/detail/TracingContext.cpp"
"raytracing/detail/TracingEngine.cpp"
"raytracing/detail/Logger.cpp"
"raytracing/detail/Geometry.cpp"
"raytracing/detail/Scene.cpp"
"raytracing/detail/TraceRay.cpp"
"raytracing/detail/TracingContext.h"
"raytracing/detail/geometry/PrivateGeometry.h"
"raytracing/detail/geometry/PrivateRootGeometry.h"
"raytracing/detail/scene/PrivateScene.h"
"raytracing/detail/intersection/Intersect.cpp"
"raytracing/detail/intersection/Intersect.h"
"raytracing/detail/intersection/acceleration/NaiveIntersection.cpp"
"raytracing/detail/intersection/acceleration/IntersectionInfo.cpp"
"raytracing/detail/intersection/acceleration/TriangleIntersection.cpp"
"raytracing/detail/intersection/acceleration/TriangleIntersection.h"
"raytracing/detail/intersection/acceleration/NaiveIntersection.h"
"raytracing/detail/intersection/acceleration/IntersectionInfo.h"
"raytracing/detail/shade_programs/ShadePrograms.cpp"
"raytracing/detail/shade_programs/StackParamsAllocator.cpp"
"raytracing/detail/shade_programs/ShadingData.cpp"
"raytracing/detail/shade_programs/ShadingData.h"
"raytracing/detail/shade_programs/ShadePrograms.h"
"raytracing/detail/shade_programs/StackParamsAllocator.h"
"samplers/GridSampler.cpp"
"samplers/GridSampler.h"
"samplers/samplers.h"
"cameras/ProjectionCamera.cpp"
"cameras/CameraConcepts.h"
"cameras/ProjectionCamera.h"
"cameras/cameras.h"
"maths/Spectrum.cpp"
"maths/maths.h"
"maths/Spectrum.h"
"maths/Vec.h"
)




set(INC 
"../")

set(LIBS 
spdlog
)


jpctr_add_library(jpc_tracer "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

