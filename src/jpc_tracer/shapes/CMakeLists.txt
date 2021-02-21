set(AUTOSRC
"Sphere.cpp"
"Triangle.cpp"
"TriangleMesh.cpp"
"Sphere.h"
"TriangleMesh.h"
)




set(INC 
"../")

set(LIBS
eigen3
core
utilities
)


jpctr_add_library(shapes "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

