set(AUTOSRC
"JPCDefaultApi.cpp"
"JPCDefaultApi.h"
)




set(INC 
"../")

set(LIBS 
    core
    films
    filters
    integrators
    lights
    renderers
    materials
    scenes
    shapes
    samplers
    cameras
    textures
    utilities
)


jpctr_add_library(jpctracer "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)
