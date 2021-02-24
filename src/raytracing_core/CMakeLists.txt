set(AUTOSRC
"Logger.cpp"
"TracingEngine.h"
"Scene.h"
"Logger.h"
"Geometry.h"
"Base.h"
"SurfaceInteraction.h"
"TraceRay.h"
)




set(INC 
"../")

set(LIBS 
eigen3
spdlog
)


jpctr_add_library(raytracing_core "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

