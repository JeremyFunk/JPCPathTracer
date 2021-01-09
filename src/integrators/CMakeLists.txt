set(AUTOSRC
"BasicIntegrator.cpp"
"LightIntegrator.cpp"
"PathIntegrator.cpp"
"BasicIntegrator.h"
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

