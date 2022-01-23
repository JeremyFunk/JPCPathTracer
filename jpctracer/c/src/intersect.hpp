#pragma once
extern "C"
{
#include "bvh.h"
#include "cglm/vec3.h"
#include "jpc_api.h"
#include "maths.h"
#include "types.h"
}
#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>

struct ray_trav_t
{
    vec3 origin;
    vec3 direction;
};
template <class T> class TD;

template <typename T1, typename T2>
// T1, T2 semiregular / regular / totally regular
struct pair
{

    T1 first;
    T2 second;

    auto operator<=>(const pair& x) const = default;
    bool operator==(const pair& x) const = default;
};

template <typename T> T& t_min(pair<T, T>& x)
{
    return x.first;
}

template <typename T> T& t_max(pair<T, T>& x)
{
    return x.second;
}
template <typename T> T t_min(const pair<T, T>& x)
{
    return x.first;
}

template <typename T> T t_max(const pair<T, T>& x)
{
    return x.second;
}

// returns t_min, t_max the distances to the origin along the ray
// if ray intersected t_min < t_max
inline pair<float, float> bounds3d_intersect(const bounds3d_t& bound,
                                             const vec3&       origin,
                                             const vec3& inverse_direction)
{
    float x_min = (bound.min[0] - origin[0]);
    float x_max = (bound.max[0] - origin[0]);

    float y_min = (bound.min[1] - origin[1]);
    float y_max = (bound.max[1] - origin[1]);

    float z_min = (bound.min[2] - origin[2]);
    float z_max = (bound.max[2] - origin[2]);

    float temp;
    if (inverse_direction[0] < 0)
    {
        temp = x_min;
        x_min = x_max;
        x_max = temp;
    }
    if (inverse_direction[1] < 0)
    {
        temp = y_min;
        y_min = y_max;
        y_max = temp;
    }
    if (inverse_direction[2] < 0)
    {
        temp = z_min;
        z_min = z_max;
        z_max = temp;
    }

    float distance = std::max(x_min, std::max(y_min, z_min));

    x_min *= inverse_direction[0];
    x_max *= inverse_direction[0];
    y_min *= inverse_direction[1];
    y_max *= inverse_direction[1];
    z_min *= inverse_direction[2];
    z_max *= inverse_direction[2];

    float t_min = std::max(x_min, std::max(y_min, z_min));

    float t_max = std::max(x_max, std::max(y_max, z_max));

    return {t_min, t_max};
}

template <typename T>
// T models an intervall
inline bool does_intersect(const pair<float, float>& x, const T& intervall)

{
    return x.first <= x.second && t_min(intervall) < x.second
           && x.first < t_max(intervall);
}

struct bounds3d_intersector
{
    vec3 origin;
    vec3 inv_dir;
    using intervall_type = pair<float, float>;
    bounds3d_intersector(ray_trav_t ray)
    {
        glm_vec3_copy(ray.origin, origin);
        for (int i = 0; i < 3; i++)
            inv_dir[i] = 1. / ray.direction[i];
    }
    template <typename I, typename O>
    // I Forward Iterator with Valuetype bounds3d
    // O Output Iterator with Valuetype std::pair<float,float>
    O operator()(I first, I last, O out)
    {
        while (first != last)
        {
            *out = bounds3d_intersect(*first, origin, inv_dir);
            first++;
            out++;
        }
        return out;
    }
};

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

template <class T>
// T models intervall
inline bool does_intersect(const float& x, const T& intervall)
{
    return t_min(intervall) < x && x < t_max(intervall);
}

float& t_min(float& x)
{
    return x;
}
float& t_max(float& x)
{
    return x;
}
float t_min(const float& x)
{
    return x;
}
float t_max(const float& x)
{
    return x;
}

struct triangle_hitpoint
{
    float distance = std::numeric_limits<float>::infinity();
    vec2  uv = {-1, -1};
};
bool operator==(const triangle_hitpoint& a, const triangle_hitpoint& b)
{
    return a.distance == b.distance && a.uv[0] == b.uv[0] && a.uv[1] == b.uv[1];
}
auto operator<=>(const triangle_hitpoint& a, const triangle_hitpoint& b)
{
    return a.distance <=> b.distance;
}

inline triangle_hitpoint triangle_intersect2(ray_trav_t ray,
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
        return triangle_hitpoint{};

    float inv_det = 1. / determinante;

    glm_vec3_sub(ray.origin, v1, diff_origin_position);

    float u = glm_vec3_dot(diff_origin_position, point_dir) * inv_det;
    glm_vec3_cross(diff_origin_position, support_vec_1, cross_op_s1);
    float v = glm_vec3_dot(ray.direction, cross_op_s1) * inv_det;

    float intersection_point_distance
        = glm_vec3_dot(support_vec_2, cross_op_s1) * inv_det;

    return triangle_hitpoint{
        .distance = intersection_point_distance,
        .uv = {u, v},
    };
}

template <class T>
inline bool does_intersect(const triangle_hitpoint& x, const T& intervall)
{
    return x.uv[0] > -ERROR_THICKNESS && x.uv[0] < 1 + ERROR_THICKNESS
           && x.uv[1] > -ERROR_THICKNESS
           && x.uv[0] + x.uv[1] < 1 + ERROR_THICKNESS
           && does_intersect(x.distance, intervall);
}
float& t_min(triangle_hitpoint& x)
{
    return x.distance;
}
float& t_max(triangle_hitpoint& x)
{
    return x.distance;
}

inline hit_point_t finalize(const triangle_hitpoint& x,
                            uint                     id,
                            ray_trav_t               ray,
                            const triangles_t&       tris,
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

inline hit_point_t finalize(const double&    t,
                            uint             id,
                            ray_trav_t       ray,
                            const spheres_t& sphs,
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
