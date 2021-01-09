set(AUTOSRC
"BVHAccel.cpp"
"BVHHelperStructs.cpp"
"BVHAccel.h"
"BVHHelperStructs.h"
)




set(INC 
"../")

set(LIBS
core
shapes
)


jpctr_add_library(accelerators "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)
