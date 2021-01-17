set(AUTOSRC
"CookTorranceBSDF.cpp"
"MicrofacetHelper.cpp"
"BeckmannDistribution.cpp"
"LambertianBRDF.cpp"
"BSDF.cpp"
"DielectricFresnel.cpp"
"SphericalCoordinates.cpp"
"MicrofacetHelper.h"
"SphericalCoordinates.h"
"CookTorranceBSDF.h"
"DielectricFresnel.h"
"IBXDF.h"
"BSDF.h"
"IFresnel.h"
"BeckmannDistribution.h"
"LambertianBRDF.h"
"IMicrofacetDistribution.h"
)




set(INC 
"../")

set(LIBS
core
samplers
)

jpctr_add_library(bsdfs "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

