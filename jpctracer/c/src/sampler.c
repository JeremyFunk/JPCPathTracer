#include "sampler.h"
#include "iterator.h"
#include "log/log.h"
#include "pcg_basic/pcg_basic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    //TODO Maby gird based sampling?
    for(uint i = 0; i<count[0]*count[1];i++)
    {
        out[i][0] = next_rand(state);
        out[i][1] = next_rand(state);
    }
}

bool sample2d_next(iterator2d* iter, sampler_state* state, vec2 rand)
{
    if (iterator_next(iter))
    {
        for (int i = 0; i < 2; i++)
        {
            float r = next_rand(state);
            float idx = iter->current_idx[i];
            
            float r_mapped = (r + idx) / iter->count[i];
            float r_range = r_mapped * (iter->range.max[i] - iter->range.min[i])
                            + iter->range.min[i];
            rand[i] = r_range;
        }
        /*
        log_info("2d rand: %f,%f\n", rand[0], rand[1]);
        log_info("x: %d, y: %d", iter->current_idx[0], iter->current_idx[1]);
        */
        return true;
    }
    return false;
}
