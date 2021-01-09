set(AUTOSRC
"arithmetic.cpp"
"ImageIO.cpp"
"objLoader.cpp"
"printerHelper.cpp"
"textureLoader.cpp"
"arithmetic.h"
"ImageIO.h"
"objLoader.h"
"printerHelper.h"
"textureLoader.h"
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

