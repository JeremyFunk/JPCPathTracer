set(AUTOSRC
"GaussianFilter.cpp"
"GaussianFilter.h"
)




set(INC 
"../")

set(LIBS 
    core
)


jpctr_add_library(filters "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)
