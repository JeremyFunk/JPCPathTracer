set(AUTOSRC
"BasicIntegrator.cpp"
"DebugIntegrator.cpp"
"LightIntegrator.cpp"
"BasicIntegrator.h"
"DebugIntegrator.h"
"LightIntegrator.h"
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

