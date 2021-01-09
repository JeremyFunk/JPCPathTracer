set(AUTOSRC
"ColorValue.cpp"
"ColorValue.h"
)




set(INC 
"../")

set(LIBS 
eigen3
textures
core
)


jpctr_add_library(color "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

