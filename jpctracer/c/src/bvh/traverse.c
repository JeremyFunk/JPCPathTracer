#include "traverse.h"
#include "../maths.h"
#include "bvh.h"
#include "jpc_api.h"
#include "shapes.h"
#include <bits/types/locale_t.h>

// params of type sphs_intersect_t
bool spheres_intersect(const ray_t* ray,
                       uint         id,
                       void*        params,
                       hit_point_t* out)
{
    sphs_intersect_t* sphs = params;
    if (sphere_intersect(ray,
                         sphs->positions[id + sphs->offset],
                         sphs->radii[id + sphs->offset],
                         &out->distance))
    {
        out->mesh_id = id + sphs->offset;
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
    tris_intersect_t* tris = params;
    uint*             v_ids = tris->verticies_ids[id + tris->offset];
    if (triangle_intersect(ray,
                           tris->verticies[v_ids[0]],
                           tris->verticies[v_ids[1]],
                           tris->verticies[v_ids[2]],
                           &out->distance,
                           out->uv))
    {
        out->mesh_id = id + tris->offset;
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
    intervallu32_t range = instance_get_range(geoms, inst);
    switch (inst.type)
    {
    case JPC_SPHERE: {
        bvh_tree_t*      local_tree = geoms->bvhtree_spheres + inst.mesh_id;
        sphs_intersect_t intersect_params = {
            .positions = geoms->spheres.positions,
            .radii = geoms->spheres.radii,
            .offset = range.min,
        };
        did_intersect = intersect_closest(local_ray,
                                          local_tree,
                                          spheres_intersect,
                                          &intersect_params,
                                          result);
        break;
    }
    case JPC_TRIANGLE: {

        bvh_tree_t*      local_tree = geoms->bvhtree_triangles + inst.mesh_id;
        tris_intersect_t intersect_params = {
            .verticies = geoms->triangles.verticies,
            .verticies_ids = geoms->triangles.verticies_ids,
            .offset = range.min,
        };
        did_intersect = intersect_closest(local_ray,
                                          local_tree,
                                          triangles_intersect,
                                          &intersect_params,
                                          result);
        break;
    }
    };
    if (did_intersect)
    {
        result->distance /= norm;
        result->mesh_id = id;
        return true;
    }
    return false;
}

typedef bool (*intersect_f)(const ray_t* ray,
                            uint         id,
                            void*        params,
                            hit_point_t* out);

bool intersect_closest(ray_t             ray,
                       const bvh_tree_t* tree,
                       intersect_f       intersect,
                       void*             params,
                       hit_point_t*      out)
{

    bvh_intersetor_closest_t intersector;
    bvh_intersect_init(tree, &ray, &intersector);

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

hit_point_t instance_finalize(hit_point_t         hit,
                              const ray_t*        ray,
                              const geometries_t* geom)
{
    uint       id = hit.instance_id;
    instance_t inst = geom->instances[id];
    uint* mat_slot_bindings = &geom->material_slots[inst.material_slot_start];

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
            hit, &local_ray, &geom->triangles, mat_slot_bindings);
        break;
    case JPC_SPHERE:

        tmp = sphere_finalize(
            hit, &local_ray, &geom->spheres, mat_slot_bindings);
        break;
    }
    hit_point_t result = tmp;
    glm_mat4_mulv3(trans, tmp.location, 1, result.location);
    glm_mat4_mulv3(trans, tmp.normal, 0, result.normal);
    glm_vec3_scale(result.normal, norm, result.normal);
    return result;
}

bool ray_intersect_c3(const geometries_t* geometries,
                      ray_t*              ray,
                      hit_point_t*        out_hitpoint)
{
#ifdef LOG_TRAVERSAL
    printf("--------------------------------------------\n");
#endif
    int         id;
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
