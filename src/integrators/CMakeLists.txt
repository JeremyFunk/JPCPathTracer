set(AUTOSRC
"PathIntegrator.cpp"
"BasicIntegrator.cpp"
"LightIntegrator.cpp"
"DebugIntegrator.cpp"
"LightIntegrator.h"
"DebugIntegrator.h"
"PathIntegrator.h"
"BasicIntegrator.h"
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

