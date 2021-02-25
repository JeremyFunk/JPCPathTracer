set(AUTOSRC
"src/TracingContext.cpp"
"src/TracingEngine.cpp"
"src/Logger.cpp"
"src/Geometry.cpp"
"src/Scene.cpp"
"src/TraceRay.cpp"
"src/TracingContext.h"
"src/geometry/PrivateGeometry.h"
"src/geometry/PrivateRootGeometry.h"
"src/scene/PrivateScene.h"
"src/intersection/Intersect.cpp"
"src/intersection/Intersect.h"
"src/intersection/acceleration/NaiveIntersection.cpp"
"src/intersection/acceleration/IntersectionInfo.cpp"
"src/intersection/acceleration/TriangleIntersection.cpp"
"src/intersection/acceleration/TriangleIntersection.h"
"src/intersection/acceleration/NaiveIntersection.h"
"src/intersection/acceleration/IntersectionInfo.h"
"src/shade_programs/ShadePrograms.cpp"
"src/shade_programs/StackParamsAllocator.cpp"
"src/shade_programs/ShadingData.cpp"
"src/shade_programs/ShadingData.h"
"src/shade_programs/ShadePrograms.h"
"src/shade_programs/StackParamsAllocator.h"
"include/TracingEngine.h"
"include/Scene.h"
"include/Logger.h"
"include/Geometry.h"
"include/Base.h"
"include/SurfaceInteraction.h"
"include/TraceRay.h"
)




set(INC 
"../")

set(LIBS 
eigen3
spdlog
)


jpctr_add_library(raytracing_core "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

