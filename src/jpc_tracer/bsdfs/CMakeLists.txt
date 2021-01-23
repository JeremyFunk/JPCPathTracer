set(AUTOSRC
"BeckmannDistribution.cpp"
"DielectricFresnel.cpp"
"SphericalCoordinates.cpp"
"LambertianBSDF.cpp"
"SphericalCoordinates.h"
"CookTorranceBSDF.h"
"DielectricFresnel.h"
"IFresnel.h"
"LambertianBSDF.h"
"BeckmannDistribution.h"
"IMicrofacetDistribution.h"
)




set(INC 
"../")

set(LIBS
core
samplers
)

jpctr_add_library(bsdfs "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

