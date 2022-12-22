#pragma once
#include <assert.h>
#include <cglm/cglm.h>
#include <stdio.h>
#define COUNT_OF(x)                                                            \
    ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static inline void printf_arrayf(uint n, const float* vec)
{
    if (n == 0)
    {
        printf("()");
        return;
    }
    printf("(%f", vec[0]);
    for (uint i = 1; i < n; i++)
        printf(",%f", vec[i]);
    printf(")");
}

static inline void printf_arrayui(uint n, uint* vec)
{
    if (n == 0)
    {
        printf("()");
        return;
    }
    printf("(%d", vec[0]);
    for (uint i = 1; i < n; i++)
        printf(",%d", vec[i]);
    printf(")");
}

#define assert_near(a, b) assert((a - b) * (a - b) < 1e-6)
