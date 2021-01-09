set(AUTOSRC
"ProjectionCamera.cpp"
"ProjectionCamera.h"
)




set(INC 
"../")

set(LIBS
eigen3
core
samplers
)


jpctr_add_library(cameras "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)
