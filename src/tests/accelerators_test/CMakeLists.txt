set(AUTOSRC
"bvh.cpp"
"lbvh_test.cpp"
)
set(INC
"../")

set(LIBS 
core
shapes
accelerators
utilities
materials
)

jpctr_add_gtest(accelerators_test "${AUTOSRC}" "${INC}" "${LIBS}" )