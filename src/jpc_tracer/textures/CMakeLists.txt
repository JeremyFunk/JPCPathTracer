set(AUTOSRC
"Texture.cpp"
"Texture.h"
)




set(INC 
"../")

set(LIBS 
    core
    eigen3
    openimageio
)


jpctr_add_library(textures "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

