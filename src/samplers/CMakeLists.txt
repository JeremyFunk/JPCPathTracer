set(AUTOSRC
"DebugSampler.cpp"
"SphereSamplers.cpp"
"StrafifiedSampler.cpp"
"DebugSampler.h"
"SphereSamplers.h"
"StratifiedSampler.h"
)




set(INC 
"../")

set(LIBS 
core
)


jpctr_add_library(samplers "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

