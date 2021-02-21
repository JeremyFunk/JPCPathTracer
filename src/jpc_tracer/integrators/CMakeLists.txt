set(AUTOSRC
"BasicIntegrator.cpp"
"DebugBsdfIntegrator.cpp"
"DebugIntegrator.cpp"
"Integration.cpp"
"LightIntegrator.cpp"
"PathIntegrator.cpp"
"BasicIntegrator.h"
"DebugBsdfIntegrator.h"
"DebugIntegrator.h"
"Integration.h"
"LightIntegrator.h"
"PathIntegrator.h"
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

