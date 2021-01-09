set(AUTOSRC
"BasicMaterial.cpp"
"GlossyMaterial.cpp"
"BasicMaterial.h"
"GlossyMaterial.h"
)




set(INC 
"../")

set(LIBS
core
bsdfs
color
)

jpctr_add_library(materials "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

