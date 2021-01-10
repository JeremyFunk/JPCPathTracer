set(AUTOSRC
"BSDF.cpp"
"LambertianBRDF.cpp"
"SphericalCoordinates.cpp"
"BSDF.h"
"IBXDF.h"
"IFresnel.h"
"IMicrofacetDistribution.h"
"LambertianBRDF.h"
"SphericalCoordinates.h"
)




set(INC 
"../")

set(LIBS
core
samplers
)

jpctr_add_library(bsdfs "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

