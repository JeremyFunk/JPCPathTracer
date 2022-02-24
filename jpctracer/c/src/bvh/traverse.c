#include "traverse.h"
#include "../maths.h"
#include "bvh.h"
#include "jpc_api.h"
#include "shapes.h"
bool spheres_intersect_closest(const ray_trav_t* ray,
                               const spheres_t*  sphs,
                               intervall_t       search_intervall,
                               const bvh_tree_t* tree,
                               int               offset_id,
                               int*              out_id,
                               float*            out_distance)
{
    bvh_intersetor_closest_t intersector;
    bvh_intersect_init(tree, search_intervall.min, ray, &intersector);

    int  id;
    bool did_intersect = false;
    while (find_closest_leaf(&id, &intersector, search_intervall.max))
    {
        id += offset_id;
        if (spheres_intersect(ray, sphs, id, search_intervall, out_distance))
        {
            *out_id = id;
            search_intervall.max = *out_distance;
            did_intersect = true;
        }
    }
    return did_intersect;
}

bool triangles_intersect_closest(const ray_trav_t*    ray,
                                 const triangles_t*   tris,
                                 intervall_t          search_intervall,
                                 const bvh_tree_t*    tree,
                                 int                  offset_id,
                                 int*                 out_id,
                                 triangle_hitpoint_t* out_hit)
{
    bvh_intersetor_closest_t intersector;
    bvh_intersect_init(tree, search_intervall.min, ray, &intersector);

    int  id;
    bool did_intersect = false;
    while (find_closest_leaf(&id, &intersector, search_intervall.max))
    {
        id += offset_id;
        if (triangles_intersect(ray, tris, id, search_intervall, out_hit))
        {
            *out_id = id;
            search_intervall.max = out_hit->distance;
            did_intersect = true;
        }
    }
    return did_intersect;
};

bool instance_intersect_closest(const ray_trav_t*    world_ray,
                                const geometries_t*  geoms,
                                uint                 id,
                                intervall_t          intervall,
                                instance_hitpoint_t* result)
{
    instance_t inst = geoms->instances[id];
    ray_trav_t local_ray;
    // transformation
    mat4 trans, inv_trans;
    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);
    float norm = ray_transform(world_ray, inv_trans, &local_ray);

    intervall_t local_intervall = {norm * intervall.min, norm * intervall.max};
    bool        did_intersect;
#ifdef LOG_TRAVERSAL
    printf("dist local: %f dist max: %f\n", local_intervall.max, intervall.max);
#endif
    intervallu32_t range = instance_get_range(geoms, inst);
    switch (inst.type)
    {
    case JPC_SPHERE: {
        bvh_tree_t* local_tree = geoms->bvhtree_spheres + inst.mesh_id;
        did_intersect = spheres_intersect_closest(&local_ray,
                                                  &geoms->spheres,
                                                  local_intervall,
                                                  local_tree,
                                                  range.min,
                                                  &result->geom_id,
                                                  &result->geom_hit.distance);

        break;
    }
    case JPC_TRIANGLE: {

        bvh_tree_t* local_tree = geoms->bvhtree_triangles + inst.mesh_id;
        did_intersect = triangles_intersect_closest(&local_ray,
                                                    &geoms->triangles,
                                                    local_intervall,
                                                    local_tree,
                                                    range.min,
                                                    &result->geom_id,
                                                    &result->geom_hit);
    }
    };
    if (did_intersect)
    {
        result->geom_hit.distance /= norm;
        return true;
    }
    return false;
}

bool instances_intersect_closest(const ray_trav_t*    ray,
                                 const geometries_t*  geoms,
                                 intervall_t          search_intervall,
                                 int*                 out_id,
                                 instance_hitpoint_t* out_hit)
{
    bvh_intersetor_closest_t intersector;
    bvh_intersect_init(
        geoms->bvhtree_instances, search_intervall.min, ray, &intersector);

    int  id;
    bool did_intersect = false;
    while (find_closest_leaf(&id, &intersector, search_intervall.max))
    {
        if (instance_intersect_closest(
                ray, geoms, id, search_intervall, out_hit))
        {
            *out_id = id;
            search_intervall.max = out_hit->geom_hit.distance;
            did_intersect = true;
        }
    }
    return did_intersect;
};

hit_point_t instance_finalize(instance_hitpoint_t hit,
                              uint                id,
                              const ray_trav_t*   ray,
                              const geometries_t* geom)
{
    instance_t inst = geom->instances[id];
    uint* mat_slot_bindings = &geom->material_slots[inst.material_slot_start];

    ray_trav_t local_ray;
    // transformation
    mat4 trans, inv_trans;
    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);

    float norm = ray_transform(ray, inv_trans, &local_ray);

    hit_point_t tmp = {.material_id = -1};
    // transform distance back
    hit.geom_hit.distance *= norm;

    switch (inst.type)
    {
    case JPC_TRIANGLE:
        tmp = triangle_finalize(&hit.geom_hit,
                                hit.geom_id,
                                &local_ray,
                                &geom->triangles,
                                mat_slot_bindings);
        break;
    case JPC_SPHERE:

        tmp = sphere_finalize(hit.geom_hit.distance,
                              hit.geom_id,
                              &local_ray,
                              &geom->spheres,
                              mat_slot_bindings);
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
    ray_trav_t          ray_trav = ray_trav_make(ray);
    int                 id;
    instance_hitpoint_t hit
        = {.geom_hit = {.distance = 0, .uv = {0, 0}}, .geom_id = -1};
    if (!instances_intersect_closest(
            &ray_trav, geometries, (intervall_t){0, ray->clip_end}, &id, &hit))
        return false;

    *out_hitpoint = instance_finalize(hit, id, &ray_trav, geometries);
    return true;
}