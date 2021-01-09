set(AUTOSRC
"BasicScene.cpp"
"BVHScene.cpp"
"BasicScene.h"
"BVHScene.h"
)




set(INC 
"../")

set(LIBS 
core
shapes
color
accelerators
materials
)


jpctr_add_library(scenes "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

