#pragma once
#include "cglm/call/vec3.h"
#include <jpc_api.h>

#include "cglm/cglm.h"
#include "jpc_api.h"
#include "types.h"
#include "bsdf.h"

typedef struct
{
    vec3 min;
    vec3 max;
} bounds3d_t;

typedef struct
{
    uint first_idx;
    uint last_idx;
    uint split_idx;
} bvh_node_t;

typedef struct bvh_tree_s
{
    int         n;
    bounds3d_t* shape_bounds; // size n
    bvh_node_t* nodes;        // size n-1
    bounds3d_t* node_bounds;  // size n-1

} bvh_tree_t;


void bounds3d_t_merge(bounds3d_t* bounds, uint n, bounds3d_t* dst);
// tree.shape_bounds should always be set
void lbvh_build(bvh_tree_t tree, vec3* centers, uint* permutation);

typedef struct{
    float* origin;
    float* direction;
} ray_t;


// updates the hitpoint
bool ray_intersect(const geometries_t* geometries, vec3 direction, hit_point_t* last_hitpoint);
typedef bool(*intersect_f)(int id, ray_t ray, float* distance, void* param);

bool traverse_bvh(const bvh_tree_t* tree, ray_t ray, float* distance,  intersect_f intersect, void* param);

#define ERROR_THICKNESS 1e-6


uint64_t rays_shadow_test(const geometries_t* geometries, bidir_scattering_t* scattering);
