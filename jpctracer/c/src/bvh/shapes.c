#include "shapes.h"
#include "../maths.h"
#include "jpc_api.h"
#include <assert.h>

ray_trav_t ray_trav_make(const ray_t* ray)
{
    ray_trav_t result;
    glm_vec3_copy((float*)ray->origin, result.origin);
    glm_vec3_copy((float*)ray->direction, result.direction);
    assert(fabs(glm_vec3_norm((float*)ray->direction) - 1) < 1e-4);
    return result;
}

float ray_transform(const ray_trav_t* ray, mat4 mat, ray_trav_t* result)
{
    glm_mat4_mulv3(mat, (float*)ray->origin, 1, result->origin);
    glm_mat4_mulv3(mat, (float*)ray->direction, 0, result->direction);
    float norm = glm_vec3_norm(result->direction);
    glm_vec3_scale(result->direction, 1. / norm, result->direction);
    return norm;
}

hit_point_t transform_hitp(const hit_point_t* hit, mat4 mat)
{
    hit_point_t result = *hit;
    glm_mat4_mulv3(mat, (float*)hit->location, 1, result.location);
    glm_mat4_mulv3(mat, (float*)hit->normal, 0, result.normal);
    return result;
}

intervall_t transform_intervall(intervall_t intervall, float scale)
{
    return (intervall_t){intervall.min * scale, intervall.max * scale};
}

intervallu32_t mesh_get_range(const uint* mesh_end_ids, uint id)
{

    intervallu32_t res;
    if (id > 0)
    {
        res.min = mesh_end_ids[id - 1];
    }
    else
    {
        res.min = 0;
    }
    res.max = mesh_end_ids[id];
    return res;
}

uint* shapes_get_mesh_end_ids(const geometries_t* geoms, geometry_type_t type)
{
    switch (type)
    {
    case JPC_SPHERE:
        return geoms->spheres.mesh_end_idx;
    case JPC_TRIANGLE:
        return geoms->triangles.mesh_end_idx;
    }
}

intervallu32_t instance_get_range(const geometries_t* geoms, instance_t inst)
{
    const uint* mesh_ends = shapes_get_mesh_end_ids(geoms, inst.type);
    return mesh_get_range(mesh_ends, inst.mesh_id);
}

float sphere_intersect(ray_trav_t ray, vec3 center, float radius)
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

triangle_hitpoint_t triangle_intersect(ray_trav_t ray,
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

hit_point_t triangle_finalize(const triangle_hitpoint_t* x,
                              uint                       id,
                              const ray_trav_t*          ray,
                              const triangles_t*         tris,
                              const uint*                mat_slots_bindings)
{
    hit_point_t hit;
    uint*       uv_ids = tris->uvs_ids[id];

    float* uv1 = tris->uvs[uv_ids[0]];
    float* uv2 = tris->uvs[uv_ids[1]];
    float* uv3 = tris->uvs[uv_ids[2]];

    uint*  norm_ids = tris->normals_ids[id];
    float* norm1 = tris->normals[norm_ids[0]];
    float* norm2 = tris->normals[norm_ids[1]];
    float* norm3 = tris->normals[norm_ids[2]];

    glm_vec3_scale((float*)ray->direction, x->distance, hit.location);
    glm_vec3_add(hit.location, (float*)ray->origin, hit.location);

    interpolate2d(uv1, uv2, uv3, x->uv[0], x->uv[1], hit.uvs);
    interpolate3d(norm1, norm2, norm3, x->uv[0], x->uv[1], hit.normal);

    uint mat_slot = tris->material_slots[id];
    hit.material_id = mat_slots_bindings[mat_slot];
    return hit;
}

hit_point_t sphere_finalize(float             t,
                            uint              id,
                            const ray_trav_t* ray,
                            const spheres_t*  sphs,
                            const uint*       mat_slots_bindings)
{
    hit_point_t hit;
    glm_vec3_normalize_to((float*)ray->direction, hit.location);
    glm_vec3_scale(hit.location, t, hit.location);
    glm_vec3_add(hit.location, (float*)ray->origin, hit.location);

    float* center = sphs->positions[id];
    glm_vec3_sub(hit.location, center, hit.normal);
    glm_vec3_normalize(hit.normal);

    hit.uvs[0] = 0; // hit.normal[0];
    hit.uvs[1] = 0; // hit.normal[1];

    uint slot_id = sphs->material_slot_id[id];
    hit.material_id = mat_slots_bindings[slot_id];
    return hit;
}

bool spheres_intersect(const ray_trav_t* ray,
                       const spheres_t*  sphs,
                       int               id,
                       intervall_t       intervall,
                       float*            result)
{
    float tmp = sphere_intersect(*ray, sphs->positions[id], sphs->radii[id]);
    if (does_intersect_point(tmp, intervall))
    {
        *result = tmp;
        return true;
    }
    return false;
}

bool triangles_intersect(const ray_trav_t*    ray,
                         const triangles_t*   tris,
                         int                  id,
                         intervall_t          intervall,
                         triangle_hitpoint_t* hitp)
{
    uint*               ids = tris->verticies_ids[id];
    triangle_hitpoint_t tmp = triangle_intersect(*ray,
                                                 tris->verticies[ids[0]],
                                                 tris->verticies[ids[1]],
                                                 tris->verticies[ids[2]]);
    if (does_intersect_triangle(tmp, intervall))
    {
        *hitp = tmp;
        return true;
    }
    return false;
}

void triangles_get_bounds(const geometries_t* geoms, bounds3d_t* bounds)
{
    const triangles_t* triangles = &geoms->triangles;
    printf("Create tri bounds\n");
    for (int i = 0; i < triangles->faces_count; i++)
    {
        const uint* vert_ids = triangles->verticies_ids[i];
        float*      v1 = triangles->verticies[vert_ids[0]];
        float*      v2 = triangles->verticies[vert_ids[1]];
        float*      v3 = triangles->verticies[vert_ids[2]];

        glm_vec3_maxv(v1, v2, bounds[i].max);
        glm_vec3_maxv(v3, bounds[i].max, bounds[i].max);

        glm_vec3_minv(v1, v2, bounds[i].min);
        glm_vec3_minv(v3, bounds[i].min, bounds[i].min);
    }
}
void spheres_get_bounds(const geometries_t* geoms, bounds3d_t* bounds)
{
    const spheres_t* spheres = &geoms->spheres;
    for (int i = 0; i < spheres->count; i++)
    {
        glm_vec3_adds(spheres->positions[i],
                      -spheres->radii[i] - ERROR_THICKNESS,
                      bounds[i].min);
        glm_vec3_adds(spheres->positions[i],
                      spheres->radii[i] + ERROR_THICKNESS,
                      bounds[i].max);
    }
};

void triangles_get_centers(const geometries_t* geoms,
                           const bounds3d_t*   bounds,
                           vec3*               centers)
{

    const triangles_t* triangles = &geoms->triangles;
    for (int i = 0; i < triangles->faces_count; i++)
    {

        const uint* vert_ids = triangles->verticies_ids[i];
        float*      v1 = triangles->verticies[vert_ids[0]];
        float*      v2 = triangles->verticies[vert_ids[1]];
        float*      v3 = triangles->verticies[vert_ids[2]];

        glm_vec3_add(v1, v2, centers[i]);
        glm_vec3_add(v3, centers[i], centers[i]);
        glm_vec3_scale(centers[i], 1. / 3., centers[i]);
    }
}

void spheres_get_centers(const geometries_t* geoms,
                         const bounds3d_t*   bounds,
                         vec3*               centers)
{

    const spheres_t* spheres = &geoms->spheres;
    for (int i = 0; i < spheres->count; i++)
        glm_vec3_copy(spheres->positions[i], centers[i]);
}