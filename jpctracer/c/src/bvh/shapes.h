#pragma once
#include "bvh_tree.h"
#include "cglm/cglm.h"
#include "jpc_api.h"

void triangles_create_bounds(const triangles_t* triangles, bounds3d_t* bounds);

void spheres_create_bounds(const spheres_t* spheres, bounds3d_t* bounds);

void triangles_get_centers(const triangles_t* triangles, vec3* centers);
void spheres_get_centers(const spheres_t* spheres, vec3* centers);