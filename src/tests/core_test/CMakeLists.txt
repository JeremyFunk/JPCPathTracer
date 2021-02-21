set(AUTOSRC
"bounds3D_test.cpp"
"bsdf_test.cpp"
"monte_carlo.cpp"
)
set(INC
"../")

set(LIBS 
    core
    rangev3
)

jpctr_add_gtest(core_test "${AUTOSRC}" "${INC}" "${LIBS}" )
