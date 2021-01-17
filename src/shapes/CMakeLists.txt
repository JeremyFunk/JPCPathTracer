set(AUTOSRC
"Sphere.cpp"
"TriangleMesh.cpp"
"Triangle.cpp"
"TriangleMesh.h"
"Sphere.h"
)




set(INC 
"../")

set(LIBS
eigen3
core
utilities
)


jpctr_add_library(shapes "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

