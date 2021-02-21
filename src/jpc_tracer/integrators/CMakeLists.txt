set(AUTOSRC
"BasicIntegrator.cpp"
"DebugBsdfIntegrator.cpp"
"DebugIntegrator.cpp"
"LightIntegrator.cpp"
"PathIntegrator.cpp"
"BasicIntegrator.h"
"DebugBsdfIntegrator.h"
"DebugIntegrator.h"
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
)


jpctr_add_library(integrators "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

