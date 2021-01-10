set(AUTOSRC
"LightIntegrator.cpp"
)
set(INC
"../")

set(LIBS 
    integrators
    scenes
    shapes
    materials
    lights
)

jpctr_add_gtest(integrators_test "${AUTOSRC}" "${INC}" "${LIBS}" )