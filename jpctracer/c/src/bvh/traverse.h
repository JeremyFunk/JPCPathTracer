#pragma once
#include "bvh.h"
#include "jpc_api.h"

typedef bool (*intersect_f)(const ray_t* ray,
                            uint         id,
                            void*        params,
                            hit_point_t* out);

bool intersect_closest(ray_t             ray,
                       const bvh_tree_t* tree,
                       intersect_f       intersect,
                       void*             params,
                       hit_point_t*      out);

bool intersect_any(ray_t             ray,
                   const bvh_tree_t* tree,
                   intersect_f       intersect,
                   void*             params,
                   hit_point_t*      out);


// params of type spheres_mesh_t
bool spheres_intersect(const ray_t* ray,
                       uint         id,
                       void*        params,
                       hit_point_t* out);

// params of type triangles_mesh_t
bool triangles_intersect(const ray_t* ray,
                         uint         id,
                         void*        params,
                         hit_point_t* out);

// params of type geometries_t
bool instance_intersect_closest(const ray_t* world_ray,
                                uint         id,
                                void*        params,
                                hit_point_t* result);

bool ray_intersect_c3(const geometries_t* geometries,
                      const ray_t*              ray,
                      hit_point_t*        out_hitpoint);

uint64_t rays_shadow_test_c3(const geometries_t* geometries,
                             const vec3*         dirs,
                             const float*        distances,
                             const vec3          origin,
                             uint                n);

