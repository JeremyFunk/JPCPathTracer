#pragma once
#include "cglm/cglm.h"
#include "jpc_api.h"
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
    int n;
    bounds3d_t* shape_bounds;    // size n
    bvh_node_t* internal_bounds; // size n-1
    bounds3d_t* internal_nodes;  // size n-1

} bvh_tree_t;