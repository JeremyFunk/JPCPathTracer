set(AUTOSRC
"DirectionalLight.cpp"
"PointLight.cpp"
"DirectionalLight.h"
"PointLight.h"
)




set(INC 
"../")

set(LIBS
core
)

jpctr_add_library(lights "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

