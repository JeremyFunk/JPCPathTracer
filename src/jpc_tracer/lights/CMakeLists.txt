set(AUTOSRC
"PointLight.cpp"
"DirectionalLight.cpp"
"DirectionalLight.h"
"PointLight.h"
)




set(INC 
"../")

set(LIBS
core
)

jpctr_add_library(lights "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

