set(AUTOSRC
"arithmetic.cpp"
"write_image.cpp"
)
set(INC
"../")

set(LIBS 
utilities
)

jpctr_add_gtest(utilities_test "${AUTOSRC}" "${INC}" "${LIBS}" )