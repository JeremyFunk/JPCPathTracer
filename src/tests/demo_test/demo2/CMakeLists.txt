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
    
)


jpctr_add_executable(demo2 "${AUTOSRC}" "${INC}" "${LIBS}")
