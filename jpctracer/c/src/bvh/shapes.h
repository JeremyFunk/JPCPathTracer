#pragma once
#include "../config.h"
#include "../types.h"
#include "bounds3d.h"
#include "cglm/cglm.h"
#include "jpc_api.h"
#include <assert.h>

typedef struct
{
    uint min;
    uint max;
} intervallu32_t;


float ray_transform(const ray_t* ray, mat4 mat, ray_t* result);

hit_point_t transform_hitp(const hit_point_t* hit, mat4 mat,float ray_norm);

intervall_t transform_intervall(intervall_t intervall, float scale);

static inline bool does_intersect_intervall(const intervall_t* a,
                                            const intervall_t* b)

{
    return a->min <= a->max && b->min < a->max && a->min < b->max;
}

static inline bool does_intersect_point(float p, intervall_t x)
{
    return x.min < p && p < x.max;
}
static inline bool does_intersect_triangle(float distance, vec2 uv,
                                           intervall_t         intervall)
{
    return does_intersect_point(distance, intervall)
           && uv[0] > -ERROR_THICKNESS && uv[0] < 1 + ERROR_THICKNESS
           && uv[1] > -ERROR_THICKNESS
           && uv[0] + uv[1] < 1 + ERROR_THICKNESS;
}

bool sphere_intersect(const ray_t *ray, vec3 center, float radius, float* distance);

bool triangle_intersect(const ray_t*  ray,
                        vec3   v1,
                        vec3   v2,
                        vec3   v3,
                        float* out_distance,
                        vec2   out_uv);

hit_point_t triangle_finalize(hit_point_t        hitp,
                              const ray_t*       ray,
                              const triangle_mesh_t* tris);

hit_point_t sphere_finalize(hit_point_t      hitp,
                            const ray_t*     ray,
                            const sphere_mesh_t* sphs);


void triangles_get_bounds(const triangle_mesh_t* tris, bounds3d_t* bounds);

void spheres_get_bounds(const sphere_mesh_t* sphs, bounds3d_t* bounds);

void triangles_get_centers(const triangle_mesh_t* tris,
                           const bounds3d_t*   bounds,
                           vec3*               centers);

void spheres_get_centers(const sphere_mesh_t* sphs,
                         const bounds3d_t*   bounds,
                         vec3*               centers);

