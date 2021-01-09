set(AUTOSRC
"main.cpp"
)
set(INC
"../")

set(LIBS 
    samplers
    cameras
    utilities
)

jpctr_add_gtest(cameras_test "${AUTOSRC}" "${INC}" "${LIBS}" )
