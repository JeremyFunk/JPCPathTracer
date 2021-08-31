#pragma once
#include<jpc_api.h> 
#include"bsdf.h"

// in the interaction the scattered dirs and the scattered_n will be change
void sample_lights(const lights_t* lights, vec2* rand_points, int n, bidir_scattering_t* interaction,
                   sampled_color_t* out_colors);


