set(AUTOSRC
"GlossyMaterial.cpp"
"BasicMaterial.cpp"
"GlossyDiffMaterial.cpp"
"GlossyDiffMaterial.h"
"GlossyMaterial.h"
"BasicMaterial.h"
)




set(INC 
"../")

set(LIBS
core
bsdfs
color
)

jpctr_add_library(materials "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

