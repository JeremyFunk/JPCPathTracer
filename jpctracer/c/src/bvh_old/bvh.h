#pragma once
#include <jpc_api.h>

#include "../bsdf.h"
#include "../config.h"
#include "../types.h"
#include "cglm/cglm.h"
#include "jpc_api.h"

//#define LOG_TRAVERSAL

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

void bounds3d_t_merge_old(bounds3d_t* bounds, uint n, bounds3d_t* dst);
// tree.shape_bounds should always be set
void lbvh_build_old(bvh_tree_t tree, vec3* centers, uint* permutation);

// updates the hitpoint
bool ray_intersect(const geometries_t* geometries,
                   ray_t*              ray,
                   hit_point_t*        out_hitpoint);
typedef bool (*intersect_f)(int id, ray_t* ray, void* param);

// updates the clip_end of the ray
bool traverse_bvh(const bvh_tree_t* tree,
                  ray_t*            ray,
                  intersect_f       intersect,
                  void*             param,
                  bool              shadow_test);

uint64_t rays_shadow_test(const geometries_t* geometries,
                          vec3*               dirs,
                          float*              distances,
                          vec3                origin,
                          uint                n);

bvh_tree_t* bvhtree_build_triangles(triangles_t tris);

bvh_tree_t* bvhtree_build_spheres(spheres_t sphs);

bvh_tree_t* bvhtree_build_instances(geometries_t* geometries);

void bvhtree_free(bvh_tree_t* tree);