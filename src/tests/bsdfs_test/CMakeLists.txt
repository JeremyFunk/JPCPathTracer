set(AUTOSRC
"diffuse_bxdf.cpp"
"Lambertian_test.cpp"
"bsdf_test.cpp"
"glossy_bxdf.cpp"
)
set(INC
"../")

set(LIBS 
    bsdfs
    samplers
    films
    utilities
    debug_helper
)

jpctr_add_gtest(bsdfs_test "${AUTOSRC}" "${INC}" "${LIBS}" )
