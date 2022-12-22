#ifndef SIMD_WIDTH
#error SIMD_WIDTH musst be defined
#endif

#if SIMD_WIDTH == 4
#include "simd-4.h"
#endif
#if SIMD_WIDTH == 2
#include "simd-2.h"
#endif
#if SIMD_WIDTH == 8
#include "simd-8.h"
#endif
