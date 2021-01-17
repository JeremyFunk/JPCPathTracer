set(AUTOSRC
"diffuse_bxdf.cpp"
"bsdf_test.cpp"
"BeckmannTest.cpp"
"glossy_bxdf.cpp"
"BxdfDebugger.cpp"
"BxdfDebugger.h"
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
