set(AUTOSRC
"BeckmannTest.cpp"
"bsdf_test.cpp"
"BxdfDebugger.cpp"
"diffuse_bxdf.cpp"
"glossy_bxdf.cpp"
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
