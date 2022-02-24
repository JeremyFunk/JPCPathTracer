#pragma once
#include "bvh.h"
bvh_tree_t lbvh_build(uint n, bounds3d_t* bounds, vec3* centers);