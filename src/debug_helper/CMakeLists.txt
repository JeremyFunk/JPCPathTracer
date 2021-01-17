set(AUTOSRC
"DebugFilm.cpp"
"DebugFilm.h"
)




set(INC 
"../")

set(LIBS 
    core
    utilities
)


jpctr_add_library(debug_helper "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)
