set(AUTOSRC
"BasicScene.cpp"
"BVHScene.cpp"
"LBVHScene.cpp"
"BasicScene.h"
"BVHScene.h"
"LBVHScene.h"
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

