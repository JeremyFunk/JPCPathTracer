set(AUTOSRC
"glossy_test.cpp"
)
set(INC
"../")

set(LIBS 
    materials
    color
)

jpctr_add_gtest(material_test "${AUTOSRC}" "${INC}" "${LIBS}" )