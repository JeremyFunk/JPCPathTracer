set(AUTOSRC
"BasicFilm.cpp"
"BasicFilm.h"
)




set(INC 
"../")

set(LIBS 
    core
    utilities
)


jpctr_add_library(films "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)
