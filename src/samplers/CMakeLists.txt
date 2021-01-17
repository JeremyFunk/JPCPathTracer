set(AUTOSRC
"StrafifiedSampler.cpp"
"SphereSamplers.cpp"
"DebugSampler.cpp"
"SphereSamplers.h"
"StratifiedSampler.h"
"DebugSampler.h"
)




set(INC 
"../")

set(LIBS 
core
)


jpctr_add_library(samplers "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

