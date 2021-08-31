#pragma once
#include <cglm/cglm.h>

typedef struct sampler_state_t sampler_state;

sampler_state* sampler_init();

void sampler_free(sampler_state* state);

void sample2d(sampler_state* state, int width, int height, vec2* out);
