set(AUTOSRC
"sphere_test.cpp"
"triangle_test.cpp"
)
set(INC
"../")

set(LIBS 
core
shapes
)

jpctr_add_gtest(shapes_test "${AUTOSRC}" "${INC}" "${LIBS}" )