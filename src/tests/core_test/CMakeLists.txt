set(AUTOSRC
"MemoryAreaTest.cpp"
"bounds3D_test.cpp"
)
set(INC
"../")

set(LIBS 
    core
    bsdfs
)

jpctr_add_gtest(core_test "${AUTOSRC}" "${INC}" "${LIBS}" )
