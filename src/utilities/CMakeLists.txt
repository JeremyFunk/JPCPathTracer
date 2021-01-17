set(AUTOSRC
"objLoader.cpp"
"textureLoader.cpp"
"ImageIO.cpp"
"arithmetic.cpp"
"printerHelper.cpp"
"objLoader.h"
"arithmetic.h"
"textureLoader.h"
"ImageIO.h"
"printerHelper.h"
)




set(INC 
"../")

set(LIBS 
    core
    shapes
    eigen3
    openimageio
    textures
    color
)


jpctr_add_library(utilities "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

