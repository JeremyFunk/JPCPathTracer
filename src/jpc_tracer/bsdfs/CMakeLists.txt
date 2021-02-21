set(AUTOSRC
"BeckmannDistribution.cpp"
"DielectricFresnel.cpp"
"LambertianBSDF.cpp"
"SphericalCoordinates.cpp"
"BeckmannDistribution.h"
"CookTorranceBSDF.h"
"DielectricFresnel.h"
"IFresnel.h"
"IMicrofacetDistribution.h"
"LambertianBSDF.h"
"SphericalCoordinates.h"
)




set(INC 
"../")

set(LIBS
core
samplers
)

jpctr_add_library(bsdfs "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

