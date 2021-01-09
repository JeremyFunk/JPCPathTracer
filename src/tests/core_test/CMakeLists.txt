set(AUTOSRC
"bounds3D_test.cpp"
"MemoryAreaTest.cpp"
)
set(INC
"../")

set(LIBS 
    core
    bsdfs
)

jpctr_add_gtest(core_test "${AUTOSRC}" "${INC}" "${LIBS}" )
