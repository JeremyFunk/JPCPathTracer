#pragma once
#include "bsdf.h"
#include <jpc_api.h>
#include "scattering.h"

void scattering_rays_reset(const lights_t* lights,uint indirect_rays, uint light_samples, scattering_rays_t* rays);

// returns the used samples
void sample_lights(const lights_t*  lights, vec2* rand_p, uint rand_count, vec3 hit_location, scattering_rays_t* scattering_rays);



scattering_rays_t scattering_rays_alloc(arena_t* arena,const lights_t* lights, uint indirect_rays, uint light_samples);