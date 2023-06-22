#include "traverse.h"
#include "../maths.h"
#include "bvh.h"
#include "jpc_api.h"
#include "shapes.h"
//#include <bits/types/locale_t.h>


// params of type sphs_intersect_t
bool spheres_intersect(const ray_t* ray,
                       uint         id,
                       void*        params,
                       hit_point_t* out)
{
    sphere_mesh_t* sphs = params;
    assert(sphs);
    if (sphere_intersect(ray,
                         sphs->geometries[id].position,
                         sphs->geometries[id].radius,
                         &out->distance))
    {
        out->mesh_id = id;
        return true;
    }
    return false;
}

// params of type tris_intersect_t
bool triangles_intersect(const ray_t* ray,
                         uint         id,
                         void*        params,
                         hit_point_t* out)
{
    triangle_mesh_t* tris = params;
    uint*            v_ids = tris->vertices_ids[id];
    if (triangle_intersect(ray,
                           tris->vertices[v_ids[0]],
                           tris->vertices[v_ids[1]],
                           tris->vertices[v_ids[2]],
                           &out->distance,
                           out->uv))
    {
        out->mesh_id = id;
        return true;
    }
    return false;
}

bool instances_intersect_closest(const ray_t* world_ray,
                                 uint         id,
                                 void*        params,
                                 hit_point_t* result)
{

    const geometries_t* geoms = params;
    instance_t          inst = geoms->instances[id];
    ray_t               local_ray;
    // transformation
    mat4 trans, inv_trans;
    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);
    float norm = ray_transform(world_ray, inv_trans, &local_ray);

    bool did_intersect;
#ifdef LOG_TRAVERSAL
    printf("dist local: %f dist max: %f\n", local_intervall.max, intervall.max);
#endif
    switch (inst.type)
    {
    case JPC_SPHERE: {

        sphere_mesh_t* sphs = &geoms->spheres[inst.mesh_id];
        bvh_tree_t*    local_tree = sphs->bvh_tree;
        did_intersect = intersect_closest(
            local_ray, local_tree, spheres_intersect, sphs, result);
        break;
    }
    case JPC_TRIANGLE: {

        triangle_mesh_t* tris = &geoms->triangles[inst.mesh_id];
        bvh_tree_t*      local_tree = tris->bvh_tree;
        did_intersect = intersect_closest(
            local_ray, local_tree, triangles_intersect, tris, result);
        break;
    }
    };
    if (did_intersect)
    {
        result->distance /= norm;
        result->instance_id = id;
        return true;
    }
    return false;
}

bool instances_intersect_any(const ray_t* world_ray,
                             uint         id,
                             void*        params,
                             hit_point_t* result)
{

    const geometries_t* geoms = params;
    instance_t          inst = geoms->instances[id];
    ray_t               local_ray;
    // transformation
    mat4 trans, inv_trans;
    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);
    ray_transform(world_ray, inv_trans, &local_ray);

#ifdef LOG_TRAVERSAL
    printf("dist local: %f dist max: %f\n", local_intervall.max, intervall.max);
#endif
    switch (inst.type)
    {
    case JPC_SPHERE: {

        sphere_mesh_t* sphs = &geoms->spheres[inst.mesh_id];
        bvh_tree_t*    local_tree = sphs->bvh_tree;
        return intersect_any(
            local_ray, local_tree, spheres_intersect, sphs, result);
        break;
    }
    case JPC_TRIANGLE: {

        triangle_mesh_t* tris = &geoms->triangles[inst.mesh_id];
        bvh_tree_t*      local_tree = tris->bvh_tree;
        return intersect_any(
            local_ray, local_tree, triangles_intersect, tris, result);
        break;
    }
    };
    assert(false);
    return false;
}
bool intersect_closest(ray_t             ray,
                       const bvh_tree_t* tree,
                       intersect_f       intersect,
                       void*             params,
                       hit_point_t*      out)
{

    bvh_intersector_closest_t intersector;
    bvh_intersect_closest_init(tree, &ray, &intersector);

    int  id;
    bool did_intersect = false;
    while (find_closest_leaf(&id, &intersector, ray.clip_end))
    {
        if (intersect(&ray, id, params, out))
        {
            ray.clip_end = out->distance;
            did_intersect = true;
        }
    }
    return did_intersect;
}

bool intersect_any(ray_t             ray,
                   const bvh_tree_t* tree,
                   intersect_f       intersect,
                   void*             params,
                   hit_point_t*      out)
{
    bvh_intersector_any_t intersector;
    bvh_intersect_any_init(tree, &ray, &intersector);
    int id;
    while (find_any_leaf(&id, &intersector))
    {
        if (intersect(&ray, id, params, out))
            return true;
    }
    return false;
}

hit_point_t instance_finalize(hit_point_t         hit,
                              const ray_t*        ray,
                              const geometries_t* geom)
{
    uint       id = hit.instance_id;
    instance_t inst = geom->instances[id];

    ray_t local_ray;
    // transformation
    mat4 trans, inv_trans;
    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);

    float norm = ray_transform(ray, inv_trans, &local_ray);

    hit_point_t tmp = {.material_id = -1};
    // transform distance back
    hit.distance *= norm;

    switch (inst.type)
    {
    case JPC_TRIANGLE:
        tmp = triangle_finalize(
            hit, &local_ray, &geom->triangles[inst.mesh_id]);
        break;
    case JPC_SPHERE:

        tmp = sphere_finalize(hit, &local_ray, &geom->spheres[inst.mesh_id]);
        break;
    }
    hit_point_t result = transform_hitp(&tmp, trans, 1. / norm);
    return result;
}

void ray_shift_origin(ray_t* ray)
{
    vec3 scaled_dir;
    glm_vec3_scale(ray->direction, 1e-6, scaled_dir);
    glm_vec3_add(ray->origin, scaled_dir, ray->origin);
}

bool ray_intersect_c3(const geometries_t* geometries,
                      const ray_t*        ray,
                      hit_point_t*        out_hitpoint)
{
#ifdef LOG_TRAVERSAL
    printf("--------------------------------------------\n");
#endif
    hit_point_t hit = {.distance = 0, .uv = {0, 0}, .mesh_id = -1};
    if (!intersect_closest(*ray,
                           geometries->bvhtree_instances,
                           instances_intersect_closest,
                           (void*)geometries,
                           &hit))
        return false;

    *out_hitpoint = instance_finalize(hit, ray, geometries);
    return true;
}

int filter_shadow_rays(const geometries_t* geometries,
                             vec3*         dirs,
                             const float*        distances,
                             const vec3          origin,
                             uint                n)
{
    assert(n < 64);

    uint free_dir_index = 0;
    for (uint i = 0; i < n; i++)
    {
        assert(free_dir_index<=i);
        ray_t       ray = make_ray(origin, dirs[i], distances[i]);
        hit_point_t tmp;
        int         result = intersect_any(ray,
                                   geometries->bvhtree_instances,
                                   instances_intersect_any,
                                   (void*)geometries,
                                   &tmp);

        if(!result)
        {
            glm_vec3_copy(dirs[i],dirs[free_dir_index]);
            free_dir_index++;
        }
    }
    return free_dir_index;
}
