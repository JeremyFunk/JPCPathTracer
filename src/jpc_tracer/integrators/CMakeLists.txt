set(AUTOSRC
"PathIntegrator.cpp"
"BasicIntegrator.cpp"
"Integration.cpp"
"DebugBsdfIntegrator.cpp"
"LightIntegrator.cpp"
"DebugIntegrator.cpp"
"LightIntegrator.h"
"DebugBsdfIntegrator.h"
"DebugIntegrator.h"
"PathIntegrator.h"
"BasicIntegrator.h"
"Integration.h"
)




set(INC 
"../")

set(LIBS 
core
utilities
textures
#eigen3
boost
)


jpctr_add_library(integrators "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

