#include "sampler.h"
#include "pcg_basic/pcg_basic.h"
#include <stdio.h>
#include <stdlib.h>

struct sampler_state_t
{
    pcg32_random_t rand_state;
};

sampler_state* sampler_init()
{
    sampler_state* state = malloc(sizeof(sampler_state));
    state->rand_state.state = rand();
    state->rand_state.inc = rand();
    return state;
}

void sampler_free(sampler_state* state)
{
    free(state);
}

float next_rand(sampler_state* state)
{
    uint32_t r = pcg32_random_r(&state->rand_state);
    return (float)r / (float)UINT32_MAX;
}

void sample2d(sampler_state* state, uint2 count, vec2* out)
{
    iterator2d iter
        = {.count = {count[0], count[1]},
           .range = (bounds2d_t){
               .min = {0, 0},
               .max = {1,1},
           },
            .current_idx = {0,0}};
    while (sample2d_next(&iter, state, *out))
    {
        out++;
    }
}

bool sample2d_next(iterator2d* iter, sampler_state* state, vec2 rand)
{

    for (int i = 0; i < 2; i++)
    {
        float r = next_rand(state);
        float idx = iter->current_idx[0];
        float r_mapped = (r + idx) / iter->count[i];
        float r_range = (r_mapped + iter->range.min[i]) / iter->range.max[i];
        rand[i] = r_range;
    }
    return iterator_next(iter);
}