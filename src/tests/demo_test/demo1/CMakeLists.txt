set(AUTOSRC
"main.cpp"
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
    scenes
    shapes
    samplers
    cameras
    textures
)


jpctr_add_executable(demo1 "${AUTOSRC}" "${INC}" "${LIBS}")
