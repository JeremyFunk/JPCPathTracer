#pragma once
#include "types.h"
typedef struct iterator2d
{
    uint2      count;
    bounds2d_t range;
    uint2      current_idx;
} iterator2d;

static inline bool iterator_next(iterator2d* iter)
{

    if (iter->current_idx[1] >= iter->count[1])
    {
        iter->current_idx[0]++;
        iter->current_idx[1] = 0;
    }
    return iter->current_idx[0] >= iter->count[0];
}

static inline iterator2d line_space2d(uint2 count, bounds2d_t range)
{
    return (iterator2d){
        .count = {count[0], count[1]},
        .range = {.min = {range.min[0], range.min[1]},
                  .max = {range.max[0], range.max[1]}},
        .current_idx = {0, 0},
    };
}

static inline iterator2d grid2d(bounds2d_t range, uint substeps)
{
    uint2 count;
    for (int i = 0; i < 2; i++)
        count[i] = (range.max[i] - range.min[i]) * substeps;
    return line_space2d(count, range);
}