#pragma once
#include "bvh.h"
#include "jpc_api.h"


typedef bool(*intersect_f)(const ray_t*ray,uint id,void* params,hit_point_t* out);

bool intersect_closest(ray_t ray,const bvh_tree_t*tree,intersect_f intersect, void*params,hit_point_t* out);


typedef struct{
    vec3* positions;
    float* radii;
    int offset;
}sphs_intersect_t;

typedef struct{
    float3* verticies;
    uint3* verticies_ids;
    int offset;
}tris_intersect_t;

//params of type sphs_intersect_t
bool spheres_intersect(const ray_t* ray,uint id, void* params, hit_point_t* out);

//params of type tris_intersect_t
bool triangles_intersect(const ray_t* ray, uint id,void* params, hit_point_t* out);

//params of type geometries_t
bool instance_intersect_closest(const ray_t* world_ray,
                                uint         id,
                                void*        params,
                                hit_point_t* result);

bool ray_intersect_c3(const geometries_t* geometries,
                      ray_t*              ray,
                      hit_point_t*        out_hitpoint);
