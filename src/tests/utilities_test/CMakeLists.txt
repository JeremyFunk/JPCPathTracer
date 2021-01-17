set(AUTOSRC
"write_image.cpp"
"arithmetic.cpp"
)
set(INC
"../")

set(LIBS 
utilities
)

jpctr_add_gtest(utilities_test "${AUTOSRC}" "${INC}" "${LIBS}" )