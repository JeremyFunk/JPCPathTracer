#include <math.h>
//#define SIMD_WIDTH 2
#if (SIMD_WIDTH == 2)
#include "../utils.h"

typedef struct
{
    float m[2];
} simdf;

// a*b-c
static inline simdf simd_fmsub_ps(simdf a, simdf b, simdf c)
{
    return (simdf){a.m[0] * b.m[0] - c.m[0], a.m[1] * b.m[1] - c.m[1]};
}
static inline simdf simd_min(simdf a, simdf b)
{
    return (simdf){
        MIN(a.m[0], b.m[0]),
        MIN(a.m[1], b.m[1]),
    };
}
static inline simdf simd_min4(simdf a, simdf b, simdf c, simdf d)
{
    simdf tmp1 = simd_min(a, b);
    simdf tmp2 = simd_min(c, d);
    return simd_min(tmp1, tmp2);
}

static inline simdf simd_max(simdf a, simdf b)
{
    return (simdf){
        MAX(a.m[0], b.m[0]),
        MAX(b.m[1], b.m[1]),
    };
}

static inline simdf simd_max4(simdf a, simdf b, simdf c, simdf d)
{
    simdf tmp1 = simd_max(a, b);
    simdf tmp2 = simd_max(c, d);
    return simd_max(tmp1, tmp2);
}

static inline simdf simd_set_ps1(float a)
{
    return (simdf){a, a};
}

static inline int simd_cmple_ps(simdf a, simdf b)
{
    int res = 0;
    res |= a.m[0] <= b.m[0] ? 1 : 0;
    res |= a.m[1] <= b.m[1] ? 2 : 0;
    return res;
}

#endif