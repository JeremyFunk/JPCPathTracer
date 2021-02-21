set(AUTOSRC
"main.cpp"
"scene_setup.cpp"
"TestLightIntegrator.cpp"
"scene_setup.h"
"TestLightIntegrator.h"
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


jpctr_add_executable(pathintegrator "${AUTOSRC}" "${INC}" "${LIBS}")
