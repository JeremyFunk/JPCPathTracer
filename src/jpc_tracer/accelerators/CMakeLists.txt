set(AUTOSRC
"BVHAccel.cpp"
"BVHHelperStructs.cpp"
"LBVH.cpp"
"BVHAccel.h"
"BVHHelperStructs.h"
"LBVH.h"
)




set(INC 
"../")

set(LIBS
core
shapes
)


jpctr_add_library(accelerators "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)
