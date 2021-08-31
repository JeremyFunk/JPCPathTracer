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

void sample2d(sampler_state* state, int width, int height, vec2* out)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float r_1 = next_rand(state);
            float r_2 = next_rand(state);
            (*out)[0] = (float)(x + r_1) / (float)width;
            (*out)[1] = (float)(y + r_2) / (float)height;
            out += 1;
        }
    }
}
