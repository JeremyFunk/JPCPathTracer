set(AUTOSRC
"BeckmannDistribution.cpp"
"BSDF.cpp"
"CookTorranceBSDF.cpp"
"DielectricFresnel.cpp"
"LambertianBRDF.cpp"
"MicrofacetHelper.cpp"
"SphericalCoordinates.cpp"
"BeckmannDistribution.h"
"BSDF.h"
"CookTorranceBSDF.h"
"DielectricFresnel.h"
"IBXDF.h"
"IFresnel.h"
"IMicrofacetDistribution.h"
"LambertianBRDF.h"
"MicrofacetHelper.h"
"SphericalCoordinates.h"
)




set(INC 
"../")

set(LIBS
core
samplers
)

jpctr_add_library(bsdfs "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

