#pragma once
#include "iterator.h"
#include "jpc_api.h"
#include "types.h"
#include <cglm/cglm.h>

typedef struct sampler_state_t sampler_state;

sampler_state* sampler_init();

void sampler_free(sampler_state* state);

bool sample2d_next(iterator2d* iter, sampler_state* state, vec2 rand);

void sample2d(sampler_state* state, uint2 count, vec2* out);
