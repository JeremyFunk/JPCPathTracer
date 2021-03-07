set(AUTOSRC
"JPCTracer.h"
"maths/Vec.h"
"raytracing/Base.h"
"raytracing/Geometry.h"
"raytracing/Logger.h"
"raytracing/Scene.h"
"raytracing/SurfaceInteraction.h"
"raytracing/TraceRay.h"
"raytracing/TracingEngine.h"
"raytracing/detail/Geometry.cpp"
"raytracing/detail/Logger.cpp"
"raytracing/detail/Scene.cpp"
"raytracing/detail/TraceRay.cpp"
"raytracing/detail/TracingContext.cpp"
"raytracing/detail/TracingEngine.cpp"
"raytracing/detail/TracingContext.h"
"raytracing/detail/geometry/PrivateGeometry.h"
"raytracing/detail/geometry/PrivateRootGeometry.h"
"raytracing/detail/intersection/Intersect.cpp"
"raytracing/detail/intersection/Intersect.h"
"raytracing/detail/intersection/acceleration/IntersectionInfo.cpp"
"raytracing/detail/intersection/acceleration/NaiveIntersection.cpp"
"raytracing/detail/intersection/acceleration/TriangleIntersection.cpp"
"raytracing/detail/intersection/acceleration/IntersectionInfo.h"
"raytracing/detail/intersection/acceleration/NaiveIntersection.h"
"raytracing/detail/intersection/acceleration/TriangleIntersection.h"
"raytracing/detail/scene/PrivateScene.h"
"raytracing/detail/shade_programs/ShadePrograms.cpp"
"raytracing/detail/shade_programs/ShadingData.cpp"
"raytracing/detail/shade_programs/StackParamsAllocator.cpp"
"raytracing/detail/shade_programs/ShadePrograms.h"
"raytracing/detail/shade_programs/ShadingData.h"
"raytracing/detail/shade_programs/StackParamsAllocator.h"
)




set(INC 
"../")

set(LIBS 
spdlog
)


jpctr_add_library(jpc_tracer "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

