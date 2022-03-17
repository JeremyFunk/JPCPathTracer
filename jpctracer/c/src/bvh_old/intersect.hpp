#pragma once
#include <bits/types/__locale_t.h>
extern "C"
{
#include "../maths.h"
#include "../types.h"
#include "../utils.h"
#include "bvh.h"
#include "cglm/vec3.h"
#include "jpc_api.h"
}
//#include "simdpp/simd.h"
#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>

//#define LOG_TRAVERSAL

struct ray_trav_t
{
    vec3 origin;
    vec3 direction;
    ray_trav_t() = default;
    ray_trav_t(ray_t ray)
    {
        glm_vec3_copy(ray.origin, this->origin);
        glm_vec3_copy(ray.direction, this->direction);
    }
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

    if (inverse_direction[0] < 0)
    {
        std::swap(x_min, x_max);
    }
    if (inverse_direction[1] < 0)
    {
        std::swap(y_min, y_max);
    }
    if (inverse_direction[2] < 0)
    {
        std::swap(z_min, z_max);
    }

    x_min *= inverse_direction[0];
    x_max *= inverse_direction[0];
    y_min *= inverse_direction[1];
    y_max *= inverse_direction[1];
    z_min *= inverse_direction[2];
    z_max *= inverse_direction[2];

    float t_min = std::max(x_min, std::max(y_min, z_min));

    float t_max = std::min(x_max, std::min(y_max, z_max));

    return {t_min, t_max};
}

template <typename T>
// T models an intervall
inline bool does_intersect(const pair<float, float>& x, const T& intervall)

{
    return x.first <= x.second && t_min(intervall) < x.second
           && x.first < t_max(intervall);
}
/*
template <std::size_t N> struct bounds3dN_t
{
    simdpp::float32<N> min[3];
    simdpp::float32<N> max[3];
};*/
// returns the index of the the first 1
// and removes this 1
inline int next_mask_idx(std::uint32_t& mask)
{
    int r = __builtin_ffs(mask);
    mask &= mask - 1;
    return r - 1;
}

template <class H, class S>
// H: node hitpoint
// Members:
//      mask :size_t
//      t_min : float array
//      t_max : float array
//      childs : randomaccessiterato
// S: forward outputiterator
// Precondition hit.mask != 0
S inline push_to_stack(H& hit, S stack)
{
    int i = next_mask_idx(hit.mask);
    *stack = {hit.childs[i], hit.t_min[i], hit.t_max[i]};
    stack++;
    return stack;
}
/*
template <size_t N>
// I: randomaccess iterator
struct bounds3d_hitpoints
{
    simdpp::float32<N> t_min;
    simdpp::float32<N> t_max;
    std::size_t        mask;
};
*/

/*
template <size_t N>
// I must be semiregular
bounds3d_hitpoints<N> bounds3d_intersect(const bounds3dN_t<N>&     bounds,
                                         const simdpp::float32<N>& inv_dir,
                                         const simdpp::float32<N>& inv_dir_org,
                                         int load_permul_min[3],
                                         int load_permul_max[3],
                                         const simdpp::float32<N>& ray_t_min,
                                         const simdpp::float32<N>& ray_t_max)

{
    using floatN = simdpp::float32<N>;

    const floatN mins[3];
    for (int i = 0; i < 3; i++)
    {
        mins[i] = simdpp::fmsub(load(&bounds.min[load_permul_min[i]]),
                                inv_dir[i],
                                inv_dir_org[i]); //(bounds.min-org)/(dir)
    }
    const floatN maxs[3];
    for (int i = 0; i < 3; i++)
    {
        maxs[i] = simdpp::fmsub(load(&bounds.max[load_permul_max[i]]),
                                inv_dir[i],
                                inv_dir_org[i]); //(bounds.max-org)/dir
    }

    bounds3d_hitpoints<N> hits;
    hits.t_min = simdpp::max(mins[0], mins[1], mins[2], ray_t_min);
    hits.t_max = simdpp::min(maxs[0], maxs[1], maxs[3], ray_t_max);
    auto mask = hits.t_min <= hits.t_max;
    hits.mask = mask.to_int();
    return hits;
}
*/

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

    return t;
}

template <class T>
// T models intervall
inline bool does_intersect(const float& x, const T& intervall)
{
    return t_min(intervall) < x && x < t_max(intervall);
}

inline float& t_min(float& x)
{
    return x;
}
inline float& t_max(float& x)
{
    return x;
}
inline float t_min(const float& x)
{
    return x;
}
inline float t_max(const float& x)
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
    return does_intersect(x.distance, intervall) && x.uv[0] > -ERROR_THICKNESS
           && x.uv[0] < 1 + ERROR_THICKNESS && x.uv[1] > -ERROR_THICKNESS
           && x.uv[0] + x.uv[1] < 1 + ERROR_THICKNESS;
}
inline float& t_min(triangle_hitpoint& x)
{
    return x.distance;
}
inline float& t_max(triangle_hitpoint& x)
{
    return x.distance;
}
inline float t_min(const triangle_hitpoint& x)
{
    return x.distance;
}
inline float t_max(const triangle_hitpoint& x)
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

    interpolate2d(uv1, uv2, uv3, x.uv[0], x.uv[1], hit.uv);
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

    hit.uv[0] = 0; // hit.normal[0];
    hit.uv[1] = 0; // hit.normal[1];

    uint slot_id = sphs.material_slot_id[id];
    hit.material_id = mat_slots_bindings[slot_id];
    return hit;
}
