#pragma once
#include "limits.h"
#include "types.h"
#include <assert.h>
typedef struct iterator2d
{
    uint2      count;
    bounds2d_t range;
    int        current_idx[2];
} iterator2d;

static inline bool iterator_next(iterator2d* iter)
{
    assert(iter->count[0] < INT_MAX);
    assert(iter->count[1] < INT_MAX);
    assert(iter->current_idx[0] >= 0);
    assert(iter->current_idx[1] >= -1);
    iter->current_idx[1]++;
    if (iter->current_idx[1] >= (int)iter->count[1])
    {
        iter->current_idx[0]++;
        iter->current_idx[1] = 0;
    }
    return iter->current_idx[0] < (int)iter->count[0];
}

static inline iterator2d line_space2d(uint2 count, bounds2d_t range)
{
    assert(count[1] < INT_MAX);
    assert(count[2] < INT_MAX);
    return (iterator2d){
        .count = {count[0], count[1]},
        .range = {.min = {range.min[0], range.min[1]},
                  .max = {range.max[0], range.max[1]}},
        .current_idx = {0, -1},
    };
}

static inline iterator2d grid2d(bounds2d_t range, uint substeps)
{
    uint2 count;
    for (int i = 0; i < 2; i++)
        count[i] = (range.max[i] - range.min[i]) * substeps;
    return line_space2d(count, range);
}
