#pragma once
#include<jpc_api.h> 
#include"bsdf.h"

// returns the used samples
int sample_lights(const lights_t* lights, vec2* rand_points, int n, hit_point_t hit_p,
                   vec3* out_directions, sampled_color_t* out_colors);


