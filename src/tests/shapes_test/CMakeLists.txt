set(AUTOSRC
"triangle_test.cpp"
"sphere_test.cpp"
)
set(INC
"../")

set(LIBS 
core
shapes
)

jpctr_add_gtest(shapes_test "${AUTOSRC}" "${INC}" "${LIBS}" )