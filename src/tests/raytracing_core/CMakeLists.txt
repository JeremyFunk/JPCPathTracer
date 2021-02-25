set(AUTOSRC
"TriangleIntersection.cpp"
"ShadingPrograms.cpp"
)
set(INC
"../")

set(LIBS 
    raytracing_core
)

jpctr_add_gtest(raytracing_core_test "${AUTOSRC}" "${INC}" "${LIBS}" )
