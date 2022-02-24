#pragma once
#include "bvh.h"
#include "jpc_api.h"

bool spheres_intersect_closest(const ray_trav_t* ray,
                               const spheres_t*  sphs,
                               intervall_t       search_intervall,
                               const bvh_tree_t* tree,
                               int               offset_id,
                               int*              out_id,
                               float*            out_distance);
typedef struct
{
    int                 geom_id;
    triangle_hitpoint_t geom_hit;
} instance_hitpoint_t;
bool instance_intersect_closest(const ray_trav_t*    world_ray,
                                const geometries_t*  geoms,
                                uint                 id,
                                intervall_t          intervall,
                                instance_hitpoint_t* result);
bool instances_intersect_closest(const ray_trav_t*    ray,
                                 const geometries_t*  geoms,
                                 intervall_t          search_intervall,
                                 int*                 out_id,
                                 instance_hitpoint_t* out_hit);
bool ray_intersect_c3(const geometries_t* geometries,
                      ray_t*              ray,
                      hit_point_t*        out_hitpoint);