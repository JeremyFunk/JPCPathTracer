set(AUTOSRC
"stratifiedsampler.cpp"
)
set(INC
"../")

set(LIBS 
    samplers
)

jpctr_add_gtest(samplers_test "${AUTOSRC}" "${INC}" "${LIBS}" )