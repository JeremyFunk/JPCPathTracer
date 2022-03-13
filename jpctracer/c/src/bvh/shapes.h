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

hit_point_t transform_hitp(const hit_point_t* hit, mat4 mat);

intervall_t transform_intervall(intervall_t intervall, float scale);

intervallu32_t mesh_get_range(const uint* mesh_end_ids, uint id);
uint* shapes_get_mesh_end_ids(const geometries_t* geoms, geometry_type_t type);
intervallu32_t instance_get_range(const geometries_t* geoms, instance_t inst);

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
                              const triangles_t* tris,
                              const uint*        mat_slots_bindings);

hit_point_t sphere_finalize(hit_point_t      hitp,
                            const ray_t*     ray,
                            const spheres_t* sphs,
                            const uint*      mat_slots_bindings);


void triangles_get_bounds(const geometries_t* geoms, bounds3d_t* bounds);

void spheres_get_bounds(const geometries_t* geoms, bounds3d_t* bounds);

void triangles_get_centers(const geometries_t* geoms,
                           const bounds3d_t*   bounds,
                           vec3*               centers);

void spheres_get_centers(const geometries_t* geoms,
                         const bounds3d_t*   bounds,
                         vec3*               centers);

typedef struct
{
    void (*get_bounds)(const geometries_t* geometries, bounds3d_t* out_bounds);
    void (*get_centers)(const geometries_t* geometries,
                        const bounds3d_t*   bounds,
                        vec3*               out_centers);
    const geometries_t* geometries;
    uint*               mesh_end_ids;
    uint                mesh_count;
    uint                count;
} shapes_t;

static inline shapes_t shapes_spheres_init(const geometries_t* geometries)
{
    return (shapes_t){
        .get_bounds = &spheres_get_bounds,
        .get_centers = &spheres_get_centers,
        .mesh_end_ids = shapes_get_mesh_end_ids(geometries, JPC_SPHERE),
        .count = geometries->spheres.count,
        .mesh_count = geometries->spheres.mesh_count,
        .geometries = geometries,
    };
};

static inline shapes_t shapes_triangles_init(const geometries_t* geometries)
{
    return (shapes_t){
        .get_bounds = &triangles_get_bounds,
        .get_centers = &triangles_get_centers,
        .mesh_end_ids = shapes_get_mesh_end_ids(geometries, JPC_TRIANGLE),
        .count = geometries->triangles.faces_count,
        .mesh_count = geometries->triangles.mesh_count,
        .geometries = geometries,
    };
}
static inline intervallu32_t shapes_get_range(const shapes_t* shape, uint id)
{
    assert(id < shape->mesh_count);
    assert(shape->mesh_count != 0);
    if (id > 0)
    {
        return (intervallu32_t){shape->mesh_end_ids[id - 1],
                                shape->mesh_end_ids[id]};
    }
    else
    {
        if (shape->mesh_count == 1)
        {
            return (intervallu32_t){0, shape->count};
        }
        else
        {
            return (intervallu32_t){0, shape->mesh_end_ids[id]};
        }
    }
}
