set(AUTOSRC
"bsdf_test.cpp"
"diffuse_bxdf.cpp"
"glossy_bxdf.cpp"
"Lambertian_test.cpp"
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
