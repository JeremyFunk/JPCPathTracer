#include "shapes.h"
#include "../maths.h"
#include "jpc_api.h"
#include <assert.h>

float ray_transform(const ray_t* ray, mat4 mat, ray_t* result)
{
    glm_mat4_mulv3(mat, (float*)ray->origin, 1, result->origin);
    glm_mat4_mulv3(mat, (float*)ray->direction, 0, result->direction);
    float norm = glm_vec3_norm(result->direction);
    glm_vec3_scale(result->direction, 1. / norm, result->direction);
    result->clip_end = ray->clip_end * norm;
    return norm;
}

hit_point_t transform_hitp(const hit_point_t* hit, mat4 mat, float ray_norm)
{
    hit_point_t result = *hit;
    glm_mat4_mulv3(mat, (float*)hit->location, 1, result.location);
    glm_mat4_mulv3(mat, (float*)hit->normal, 0, result.normal);
    glm_vec3_normalize((float*)hit->normal);
    result.distance *= ray_norm;
    return result;
}

intervall_t transform_intervall(intervall_t intervall, float scale)
{
    return (intervall_t){intervall.min * scale, intervall.max * scale};
}

bool sphere_intersect(const ray_t* ray,
                      vec3         center,
                      float        radius,
                      float*       out_distance)
{
    float radius2 = radius * radius;

    float t, t0, t1; // soluations for t if the ray intersects
    // geometric solution

    vec3 L;
    glm_vec3_sub((float*)ray->origin, center, L);

    float a = glm_vec3_norm2((float*)ray->direction);
    float b = 2 * glm_vec3_dot((float*)ray->direction, L);
    float c = glm_vec3_norm2(L) - radius2;
    if (!solveQuadratic(a, b, c, &t0, &t1))
    {
#ifdef LOG_TRAVERSAL
        printf("inter false\n");
#endif
        return false;
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
            return false;
        }
    }

    t = t0 - ERROR_THICKNESS;

    if (does_intersect_point(t, (intervall_t){BVH_MIN_DISTANCE, ray->clip_end}))
    {
        *out_distance = t;
        return true;
    }
    return false;
}

bool triangle_intersect(const ray_t* ray,
                        vec3         v1,
                        vec3         v2,
                        vec3         v3,
                        float*       out_distance,
                        vec2         out_uv)
{
    //M�ller-Trumbore intersection algorithm
    float epsilion = 1e-6;
    vec3  support_vec_1, support_vec_2;
    vec3  point_dir, diff_origin_position, cross_op_s1;

    glm_vec3_sub(v2, v1, support_vec_1);
    glm_vec3_sub(v3, v1, support_vec_2);

    glm_vec3_cross((float*)ray->direction, support_vec_2, point_dir);
    float determinante = glm_vec3_dot(support_vec_1, point_dir);
    if (fabs(determinante) < epsilion)
        return false;

    float inv_det = 1. / determinante;

    glm_vec3_sub((float*)ray->origin, v1, diff_origin_position);

    vec2 uv;
    uv[0] = glm_vec3_dot(diff_origin_position, point_dir) * inv_det;
    glm_vec3_cross(diff_origin_position, support_vec_1, cross_op_s1);
    uv[1] = glm_vec3_dot((float*)ray->direction, cross_op_s1) * inv_det;

    float intersection_point_distance
        = glm_vec3_dot(support_vec_2, cross_op_s1) * inv_det;

    if (does_intersect_triangle(
            intersection_point_distance, uv, (intervall_t){BVH_MIN_DISTANCE, ray->clip_end}))
    {
        *out_distance = intersection_point_distance;
        out_uv[0] = uv[0];
        out_uv[1] = uv[1];
        return true;
    }
    return false;
}

hit_point_t triangle_finalize(hit_point_t            x,
                              const ray_t*           ray,
                              const triangle_mesh_t* tris)
{
    uint        id = x.mesh_id;
    hit_point_t hit = x;
    uint*       uv_ids = tris->uvs_ids[id];

    float* uv1 = tris->uvs[uv_ids[0]];
    float* uv2 = tris->uvs[uv_ids[1]];
    float* uv3 = tris->uvs[uv_ids[2]];

    uint*  norm_ids = tris->normals_ids[id];
    float* norm1 = tris->normals[norm_ids[0]];
    float* norm2 = tris->normals[norm_ids[1]];
    float* norm3 = tris->normals[norm_ids[2]];

    glm_vec3_scale((float*)ray->direction, x.distance, hit.location);
    glm_vec3_add(hit.location, (float*)ray->origin, hit.location);

    interpolate2d(uv1, uv2, uv3, x.uv[0], x.uv[1], hit.uv);
    interpolate3d(norm1, norm2, norm3, x.uv[0], x.uv[1], hit.normal);

    hit.material_id = tris->material_ids[id];
    return hit;
}

hit_point_t sphere_finalize(hit_point_t          x,
                            const ray_t*         ray,
                            const sphere_mesh_t* sphs)
{
    hit_point_t hit = x;
    float       t = x.distance;
    glm_vec3_normalize_to((float*)ray->direction, hit.location);
    glm_vec3_scale(hit.location, t, hit.location);
    glm_vec3_add(hit.location, (float*)ray->origin, hit.location);

    float* center = sphs->geometries[x.mesh_id].position;
    glm_vec3_sub(hit.location, center, hit.normal);
    glm_vec3_normalize(hit.normal);

    hit.uv[0] = 0; // hit.normal[0];
    hit.uv[1] = 0; // hit.normal[1];

    hit.material_id = sphs->material_ids[x.mesh_id];
    return hit;
}

void triangles_get_bounds(const triangle_mesh_t* tris, bounds3d_t* bounds)
{
    printf("Create tri bounds\n");
    for (uint i = 0; i < tris->faces_count; i++)
    {
        const uint* vert_ids = tris->vertices_ids[i];
        float*      v1 = tris->vertices[vert_ids[0]];
        float*      v2 = tris->vertices[vert_ids[1]];
        float*      v3 = tris->vertices[vert_ids[2]];

        glm_vec3_maxv(v1, v2, bounds[i].max);
        glm_vec3_maxv(v3, bounds[i].max, bounds[i].max);

        glm_vec3_minv(v1, v2, bounds[i].min);
        glm_vec3_minv(v3, bounds[i].min, bounds[i].min);
    }
}
void spheres_get_bounds(const sphere_mesh_t* sphs, bounds3d_t* bounds)
{
    for (uint i = 0; i < sphs->count; i++)
    {
        glm_vec3_adds(sphs->geometries[i].position,
                      -sphs->geometries[i].radius - ERROR_THICKNESS,
                      bounds[i].min);
        glm_vec3_adds(sphs->geometries[i].position,
                      sphs->geometries[i].radius + ERROR_THICKNESS,
                      bounds[i].max);
    }
}

void triangles_get_centers(const triangle_mesh_t* tris,
                           const bounds3d_t*      bounds,
                           vec3*                  centers)
{
    assert(bounds);
    for (uint i = 0; i < tris->faces_count; i++)
    {

        const uint* vert_ids = tris->vertices_ids[i];
        float*      v1 = tris->vertices[vert_ids[0]];
        float*      v2 = tris->vertices[vert_ids[1]];
        float*      v3 = tris->vertices[vert_ids[2]];

        glm_vec3_add(v1, v2, centers[i]);
        glm_vec3_add(v3, centers[i], centers[i]);
        glm_vec3_scale(centers[i], 1. / 3., centers[i]);
    }
}

void spheres_get_centers(const sphere_mesh_t* sphs,
                         const bounds3d_t*    bounds,
                         vec3*                centers)
{
    assert(bounds);
    for (uint i = 0; i < sphs->count; i++)
        glm_vec3_copy(sphs->geometries[i].position, centers[i]);
}
