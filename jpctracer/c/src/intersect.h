#pragma once
#include "bvh.h"
#include "jpc_api.h"
#include "types.h"
#include "utils.h"
#include <math.h>
#include "maths.h"

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

typedef struct
{
    vec3 origin;
    vec3 inverse_direction;
} ray_trav_bounds_t;

inline ray_trav_bounds_t ray_trav_bounds_make(ray_trav_t ray)
{
    ray_trav_bounds_t result;
    glm_vec3_copy(ray.origin, result.origin);
    for (int i = 0; i < 3; i++)
    {
        result.inverse_direction[i] = 1. / ray.direction[i];
    }
    return result;
}

inline intervall_t bounds3d_intersect(bounds3d_t bound, ray_trav_bounds_t ray)
{
    float x_min = (bound.min[0] - ray.origin[0]);
    float x_max = (bound.max[0] - ray.origin[0]);

    float y_min = (bound.min[1] - ray.origin[1]);
    float y_max = (bound.max[1] - ray.origin[1]);

    float z_min = (bound.min[2] - ray.origin[2]);
    float z_max = (bound.max[2] - ray.origin[2]);

    float temp;

    if (ray.inverse_direction[0] < 0)
    {
        temp = x_min;
        x_min = x_max;
        x_max = temp;
    }
    if (ray.inverse_direction[1] < 0)
    {
        temp = y_min;
        y_min = y_max;
        y_max = temp;
    }
    if (ray.inverse_direction[2] < 0)
    {
        temp = z_min;
        z_min = z_max;
        z_max = temp;
    }

    x_min *= ray.inverse_direction[0];
    x_max *= ray.inverse_direction[0];
    y_min *= ray.inverse_direction[1];
    y_max *= ray.inverse_direction[1];
    z_min *= ray.inverse_direction[2];
    z_max *= ray.inverse_direction[2];

    float t_min = MAX(x_min, MAX(y_min, z_min));

    float t_max = MIN(x_max, MIN(y_max, z_max));

    return (intervall_t){t_min, t_max};
}

inline bool does_intersect_intervall(intervall_t a, intervall_t b)

{
    return a.min <= b.max && b.min < a.max && a.min < b.max;
}

inline bool does_intersect_point(float p, intervall_t x)
{
    return x.min <= p && p <= x.max;
}

inline float sphere_intersect(ray_trav_t ray, vec3 center, float radius)
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
        return std::numeric_limits<float>::quiet_NaN();
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
            return std::numeric_limits<float>::quiet_NaN();
        }
    }

    t = t0 - ERROR_THICKNESS;

#ifdef LOG_TRAVERSAL
    printf("t: %f, max_distance: %f norm: %f\n",
           t,
           ray->clip_end,
           glm_vec3_norm(ray->direction));
#endif
    return t;
}
typedef struct
{
    float distance;
    vec2  uv;
} triangle_hitpoint_t;

inline triangle_hitpoint_t triangle_intersect2(ray_trav_t ray,
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

inline bool does_intersect_triangle(triangle_hitpoint_t x,intervall_t intervall)
{
    return does_intersect_point(x.distance, intervall) && x.uv[0] > -ERROR_THICKNESS
           && x.uv[0] < 1 + ERROR_THICKNESS && x.uv[1] > -ERROR_THICKNESS
           && x.uv[0] + x.uv[1] < 1 + ERROR_THICKNESS;
}


inline hit_point_t triangles_finalize(triangle_hitpoint_t x,
                            uint                     id,
                            ray_trav_t               ray,
                            triangles_t       tris,
                            uint*                    mat_slots_bindings)
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

    interpolate2d(uv1, uv2, uv3, x.uv[0], x.uv[1], hit.uvs);
    interpolate3d(norm1, norm2, norm3, x.uv[0], x.uv[1], hit.normal);

    uint mat_slot = tris.material_slots[id];
    hit.material_id = mat_slots_bindings[mat_slot];
    return hit;
}

inline hit_point_t spheres_finalize(float   t,
                            uint             id,
                            ray_trav_t       ray,
                            spheres_t sphs,
                            uint*            mat_slots_bindings)
{
    hit_point_t hit;
    glm_vec3_normalize_to(ray.direction, hit.location);
    glm_vec3_scale(hit.location, t, hit.location);
    glm_vec3_add(hit.location, ray.origin, hit.location);

    float* center = sphs.positions[id];
    glm_vec3_sub(hit.location, center, hit.normal);
    glm_vec3_normalize(hit.normal);

    hit.uvs[0] = hit.normal[0];
    hit.uvs[1] = hit.normal[1];

    uint slot_id = sphs.material_slot_id[id];
    hit.material_id = mat_slots_bindings[slot_id];
    return hit;
}

