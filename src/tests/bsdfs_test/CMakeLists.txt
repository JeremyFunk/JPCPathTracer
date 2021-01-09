set(AUTOSRC
"bsdf_test.cpp"
)
set(INC
"../")

set(LIBS 
    bsdfs
    samplers
)

jpctr_add_gtest(bsdfs_test "${AUTOSRC}" "${INC}" "${LIBS}" )
