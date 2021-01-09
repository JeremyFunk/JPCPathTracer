set(AUTOSRC
"BeckmannDistribution.cpp"
"BSDF.cpp"
"DiffuseBxdf.cpp"
"SphericalCoordinates.cpp"
"TorranceSparrowBRDF.cpp"
"BeckmannDistribution.h"
"BSDF.h"
"DiffuseBxdf.h"
"IBXDF.h"
"IFresnel.h"
"IMicrofacetDistribution.h"
"SphericalCoordinates.h"
"TorranceSparrowBRDF.h"
)




set(INC 
"../")

set(LIBS
core
samplers
)

jpctr_add_library(bsdfs "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

