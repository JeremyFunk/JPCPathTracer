set(AUTOSRC
"BVHAccel.cpp"
"BVHHelperStructs.cpp"
"BVHHelperStructs.h"
"BVHAccel.h"
)




set(INC 
"../")

set(LIBS
core
shapes
)


jpctr_add_library(accelerators "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)
