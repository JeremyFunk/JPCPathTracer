#pragma once
#include "../maths.h"
#include "../types.h"
#include "../utils.h"
#include "bvh.h"
#include "cglm/call/vec3.h"
#include "jpc_api.h"
#include <assert.h>
#include <math.h>

typedef struct
{
    float min;
    float max;
} intervall_t;

typedef struct
{
    vec3 origin;
    vec3 direction;
} ray_trav_t;

static inline ray_trav_t ray_trav_make(ray_t ray)
{
    ray_trav_t result;
    glm_vec3_copy(ray.origin, result.origin);
    glm_vec3_copy(ray.direction, result.direction);
    assert(fabs(glmc_vec3_norm(ray.direction) - 1) < 1e-4);
    return result;
}

typedef struct
{
    vec3 origin;
    vec3 inverse_direction;
} ray_trav_bounds_t;
static inline void log_ray_trav(ray_trav_t ray)
{
#ifdef LOG_TRAVERSAL
    printf("org: ");
    printf_arrayf(3, ray.origin);
    printf("dir: ");
    printf_arrayf(3, ray.direction);
    printf("\n");
#endif
}

static inline void log_sphere(vec3 center, float radius)
{
#ifdef LOG_TRAVERSAL
    printf("center: ");
    printf_arrayf(3, center);
    printf("radius: %f\n", radius);
#endif
}

static inline void log_intervall(intervall_t inter)
{

#ifdef LOG_TRAVERSAL
    printf("min: %f, %f\n", inter.min, inter.max);
#endif
};
static inline ray_trav_bounds_t ray_trav_bounds_make_old(ray_trav_t ray)
{
    ray_trav_bounds_t result;
    glm_vec3_copy(ray.origin, result.origin);
    for (int i = 0; i < 3; i++)
    {
        result.inverse_direction[i] = 1. / ray.direction[i];
    }
    return result;
}
static inline float ray_transform(ray_trav_t ray, mat4 mat, ray_trav_t* result)
{
    glm_mat4_mulv3(mat, ray.origin, 1, result->origin);
    glm_mat4_mulv3(mat, ray.direction, 0, result->direction);
    float norm = glm_vec3_norm(result->direction);
    glm_vec3_scale(result->direction, 1. / norm, result->direction);
    return norm;
}

static inline hit_point_t transform_hitp(hit_point_t hit, mat4 mat)
{
    hit_point_t result = hit;
    glm_mat4_mulv3(mat, hit.location, 1, result.location);
    glm_mat4_mulv3(mat, hit.normal, 0, result.normal);
    return result;
};

static inline intervall_t transform_intervall(intervall_t intervall,
                                              float       scale)
{
    return (intervall_t){intervall.min * scale, intervall.max * scale};
}

typedef struct
{
    uint min;
    uint max;
} intervallu32_t;
static inline intervallu32_t get_mesh_range(const geometries_t* geoms,
                                            instance_t          inst)
{
    uint* mesh_ends;
    switch (inst.type)
    {
    case JPC_SPHERE:
        mesh_ends = geoms->spheres.mesh_end_idx;
        break;
    case JPC_TRIANGLE:
        mesh_ends = geoms->triangles.mesh_end_idx;
        break;
    }
    intervallu32_t res;
    if (inst.mesh_id > 0)
    {
        res.min = mesh_ends[inst.mesh_id - 1];
    }
    else
    {
        res.min = 0;
    }
    res.max = mesh_ends[inst.mesh_id];
    return res;
}
static inline intervall_t bounds3d_intersect(const bounds3d_t*        bound,
                                             const ray_trav_bounds_t* ray)
{
    float x_min = (bound->min[0] - ray->origin[0]);
    float x_max = (bound->max[0] - ray->origin[0]);

    float y_min = (bound->min[1] - ray->origin[1]);
    float y_max = (bound->max[1] - ray->origin[1]);

    float z_min = (bound->min[2] - ray->origin[2]);
    float z_max = (bound->max[2] - ray->origin[2]);

    float temp;

    if (ray->inverse_direction[0] < 0)
    {
        temp = x_min;
        x_min = x_max;
        x_max = temp;
    }
    if (ray->inverse_direction[1] < 0)
    {
        temp = y_min;
        y_min = y_max;
        y_max = temp;
    }
    if (ray->inverse_direction[2] < 0)
    {
        temp = z_min;
        z_min = z_max;
        z_max = temp;
    }

    x_min *= ray->inverse_direction[0];
    x_max *= ray->inverse_direction[0];
    y_min *= ray->inverse_direction[1];
    y_max *= ray->inverse_direction[1];
    z_min *= ray->inverse_direction[2];
    z_max *= ray->inverse_direction[2];

    float t_min = MAX(x_min, MAX(y_min, z_min));

    float t_max = MIN(x_max, MIN(y_max, z_max));

    return (intervall_t){t_min, t_max};
}

static inline bool does_intersect_intervall(const intervall_t* a,
                                            const intervall_t* b)

{
    return a->min <= a->max && b->min < a->max && a->min < b->max;
}

static inline bool does_intersect_point(float p, intervall_t x)
{
    return x.min < p && p < x.max;
}

static inline float sphere_intersect(ray_trav_t ray, vec3 center, float radius)
{
    float radius2 = radius * radius;

    float t, t0, t1; // soluations for t if the ray intersects
    // geometric solution

    vec3 L;
    glm_vec3_sub(ray.origin, center, L);

    float a = glm_vec3_norm2(ray.direction);
    float b = 2 * glm_vec3_dot(ray.direction, L);
    float c = glm_vec3_norm2(L) - radius2;
    if (!solveQuadratic(a, b, c, &t0, &t1))
    {
#ifdef LOG_TRAVERSAL
        printf("inter false\n");
#endif
        return NAN;
    }

    if (t0 > t1)
    {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if (t0 < 0)
    {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0)
        {

#ifdef LOG_TRAVERSAL
            printf("inter false\n");
#endif
            return NAN;
        }
    }

    t = t0 - ERROR_THICKNESS;

    return t;
}
typedef struct
{
    float distance;
    vec2  uv;
} triangle_hitpoint_t;

static inline triangle_hitpoint_t triangle_intersect2(ray_trav_t ray,
                                                      vec3       v1,
                                                      vec3       v2,
                                                      vec3       v3)
{
    float epsilion = 1e-6;
    vec3  support_vec_1, support_vec_2;
    vec3  point_dir, diff_origin_position, cross_op_s1;

    glm_vec3_sub(v2, v1, support_vec_1);
    glm_vec3_sub(v3, v1, support_vec_2);

    glm_vec3_cross(ray.direction, support_vec_2, point_dir);
    float determinante = glm_vec3_dot(support_vec_1, point_dir);
    if (determinante < epsilion)
        return (triangle_hitpoint_t){FP_NAN};

    float inv_det = 1. / determinante;

    glm_vec3_sub(ray.origin, v1, diff_origin_position);

    float u = glm_vec3_dot(diff_origin_position, point_dir) * inv_det;
    glm_vec3_cross(diff_origin_position, support_vec_1, cross_op_s1);
    float v = glm_vec3_dot(ray.direction, cross_op_s1) * inv_det;

    float intersection_point_distance
        = glm_vec3_dot(support_vec_2, cross_op_s1) * inv_det;

    return (triangle_hitpoint_t){
        .distance = intersection_point_distance,
        .uv = {u, v},
    };
}

static inline bool does_intersect_triangle(triangle_hitpoint_t x,
                                           intervall_t         intervall)
{
    return does_intersect_point(x.distance, intervall)
           && x.uv[0] > -ERROR_THICKNESS && x.uv[0] < 1 + ERROR_THICKNESS
           && x.uv[1] > -ERROR_THICKNESS
           && x.uv[0] + x.uv[1] < 1 + ERROR_THICKNESS;
}

static inline hit_point_t triangles_finalize(triangle_hitpoint_t x,
                                             uint                id,
                                             ray_trav_t          ray,
                                             triangles_t         tris,
                                             uint* mat_slots_bindings)
{
    hit_point_t hit;
    uint*       uv_ids = tris.uvs_ids[id];

    float* uv1 = tris.uvs[uv_ids[0]];
    float* uv2 = tris.uvs[uv_ids[1]];
    float* uv3 = tris.uvs[uv_ids[2]];

    uint*  norm_ids = tris.normals_ids[id];
    float* norm1 = tris.normals[norm_ids[0]];
    float* norm2 = tris.normals[norm_ids[1]];
    float* norm3 = tris.normals[norm_ids[2]];

    glm_vec3_scale(ray.direction, x.distance, hit.location);
    glm_vec3_add(hit.location, ray.origin, hit.location);

    interpolate2d(uv1, uv2, uv3, x.uv[0], x.uv[1], hit.uv);
    interpolate3d(norm1, norm2, norm3, x.uv[0], x.uv[1], hit.normal);

    uint mat_slot = tris.material_slots[id];
    hit.material_id = mat_slots_bindings[mat_slot];
    return hit;
}

static inline hit_point_t spheres_finalize(float      t,
                                           uint       id,
                                           ray_trav_t ray,
                                           spheres_t  sphs,
                                           uint*      mat_slots_bindings)
{
    hit_point_t hit;
    glm_vec3_normalize_to(ray.direction, hit.location);
    glm_vec3_scale(hit.location, t, hit.location);
    glm_vec3_add(hit.location, ray.origin, hit.location);

    float* center = sphs.positions[id];
    glm_vec3_sub(hit.location, center, hit.normal);
    glm_vec3_normalize(hit.normal);

    hit.uv[0] = 0; // hit.normal[0];
    hit.uv[1] = 0; // hit.normal[1];

    uint slot_id = sphs.material_slot_id[id];
    hit.material_id = mat_slots_bindings[slot_id];
    return hit;
}
bool ray_intersect_naive(const geometries_t* geometries,
                         ray_t*              ray,
                         hit_point_t*        out_hitpoint);
