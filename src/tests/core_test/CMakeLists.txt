set(AUTOSRC
"monte_carlo.cpp"
"bounds3D_test.cpp"
"bsdf_test.cpp"
)
set(INC
"../")

set(LIBS 
    core
)

jpctr_add_gtest(core_test "${AUTOSRC}" "${INC}" "${LIBS}" )
