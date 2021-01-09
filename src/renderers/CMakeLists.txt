set(AUTOSRC
"BasicRenderer.cpp"
"BasicRenderer.h"
)




set(INC 
"../")

set(LIBS
core
)

jpctr_add_library(renderers "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)


find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(renderers OpenMP::OpenMP_CXX)
endif()