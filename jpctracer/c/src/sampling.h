#pragma once
#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "cglm/struct/vec2.h"
#include <assert.h>
void concentric_sample_disk(vec2 random_point, vec2* out);


void cosinus_sample_hemisphere(vec2 random_point, vec3* out);

// updates rand_p, to be reused
int sample_discrete(float* pdfs, int n, float* rand_p);

