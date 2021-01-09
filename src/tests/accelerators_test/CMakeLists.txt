set(AUTOSRC
"bvh.cpp"
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