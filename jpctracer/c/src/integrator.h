
#pragma once

#include "cglm/types.h"
#include "sampler.h"
#include "types.h"

typedef struct integrator_s integrator_t;

integrator_t* integrator_init(int            max_depth,
                              const scene_t* scene,
                              sampler_state* sampler,
                              uint           light_samples,
                              int passes);

void integrator_free(integrator_t* integrator);

void integrate(integrator_t* integrator, ray_t ray, float* color);
