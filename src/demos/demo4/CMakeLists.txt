set(AUTOSRC
"main.cpp"
)




set(INC 
"../")

set(LIBS 
    jpctracer
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


jpctr_add_executable(demo4 "${AUTOSRC}" "${INC}" "${LIBS}")
