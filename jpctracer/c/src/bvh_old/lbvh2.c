#include "intersect.h"
#include "jpc_api.h"
#include "lbvh.h"

bool spheres_intersect_old(const ray_trav_t* ray,
                           const spheres_t*  sphs,
                           intervall_t       intervall,
                           int               id,
                           float*            result)
{
#ifdef LOG_TRAVERSAL
    printf("sphere intersect\n");
    printf("center: ");
    printf_arrayf(3, sphs.positions[id]);
    printf(" radius: %f\n", sphs.radii[id]);
    printf("Ray: org: ");
    printf_arrayf(3, ray.origin);
    printf(" dir: ");
    printf_arrayf(3, ray.direction);
    printf("\n");
    printf("intervall: %f,%f\n", intervall.min, intervall.max);
#endif
    float tmp = sphere_intersect(*ray, sphs->positions[id], sphs->radii[id]);
    if (does_intersect_point(tmp, intervall))
    {

#ifdef LOG_TRAVERSAL
        printf("Did intersect. Distance: %f\n", tmp);
#endif
        *result = tmp;
        return true;
    }

#ifdef LOG_TRAVERSAL
    printf("Did not intersect:\n");
#endif
    return false;
}

bool spheres_intesect_closest_old(const ray_trav_t* ray,
                                  const spheres_t*  sphs,
                                  intervall_t       intervall,
                                  const bvh_tree_t* tree,
                                  int               index_offset,
                                  int*              out_id,
                                  float*            out_distance)
{
    bvh_intersetor_closest_t intersector;
    float                    max_distance = intervall.max;
    // check not nessasary
    bvh_intersect_init(*tree, intervall.min, *ray, &intersector);

    int  i;
    bool did_intersect = false;
    while (find_closest_leaf(&i, &intersector, intervall.max))
    {
        did_intersect |= spheres_intersect_old(
            ray, sphs, intervall, i + index_offset, &intervall.max);
    }
    if (did_intersect)
    {
        *out_id = i + index_offset;
        *out_distance = intervall.max;
        return true;
    }
    return false;
}

typedef struct
{
    const spheres_t* sps;
    const int        index_offset;
    float*           hit_p;
    int*             out_id;
} sphs_intersect_params;

bool _sphs_intersect(void* restrict param,
                     const ray_trav_t* ray,
                     intervall_t       intervall,
                     int               id,
                     float* restrict out_max_dist)
{
    sphs_intersect_params* p = param;
    id += p->index_offset;
    float hit
        = sphere_intersect(*ray, p->sps->positions[id], p->sps->radii[id]);
    if (does_intersect_point(hit, intervall))
    {
        *p->hit_p = hit;
        *out_max_dist = hit;
        *p->out_id = id;
        return true;
    }
    return false;
}

bool spheres_intesect_closest2(const ray_trav_t* ray,
                               const spheres_t*  sphs,
                               intervall_t       intervall,
                               const bvh_tree_t* tree,
                               int               index_offset,
                               int*              out_id,
                               float*            out_distance)
{
    sphs_intersect_params p = {
        .sps = sphs,
        .hit_p = out_distance,
        .index_offset = index_offset,
        .out_id = out_id,
    };
    return find_closest_leaf2(tree, ray, intervall, _sphs_intersect, &p);
}

bool spheres_intesect_any(const ray_trav_t* ray,
                          const spheres_t*  sphs,
                          intervall_t       intervall,
                          const bvh_tree_t* tree,

                          int index_offset)
{
    bvh_intersetor_any_t intersector;
    float                max_distance = intervall.max;
    bvh_intersect_any_init(*tree, *ray, &intersector);

    int i;
    while (find_any_leaf(&i, &intersector, intervall))
    {
        float tmp;
        if (spheres_intersect_old(ray, sphs, intervall, i + index_offset, &tmp))
            return true;
    }
    return false;
}
bool triangles_intersect_old(const ray_trav_t*    ray,
                             const triangles_t*   tris,
                             int                  id,
                             intervall_t          intervall,
                             triangle_hitpoint_t* hitp)
{
    uint*               ids = tris->verticies_ids[id];
    triangle_hitpoint_t tmp = triangle_intersect2(*ray,
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

bool triangles_intersect_closest_old(const ray_trav_t*  ray,
                                     const triangles_t* tris,
                                     intervall_t        intervall,
                                     const bvh_tree_t*  tree,

                                     int                  index_offset,
                                     int*                 out_id,
                                     triangle_hitpoint_t* out_hitp)
{
    bvh_intersetor_closest_t intersector;
    bvh_intersect_init(*tree, intervall.min, *ray, &intersector);
    float max_distance = intervall.max;

    int i;
    while (find_closest_leaf(&i, &intersector, intervall.max))
    {
        if (triangles_intersect_old(
                ray, tris, i + index_offset, intervall, out_hitp))
            intervall.max = out_hitp->distance;
    }
    if (max_distance < intervall.max)
    {
        *out_id = i + index_offset;
        return true;
    }
    return false;
}

bool triangles_intersect_any(const ray_trav_t*  ray,
                             const triangles_t* tris,
                             intervall_t        intervall,
                             const bvh_tree_t*  tree,
                             int                index_offset)
{
    bvh_intersetor_any_t intersector;
    float                max_distance = intervall.max;
    bvh_intersect_any_init(*tree, *ray, &intersector);

    int i;
    while (find_any_leaf(&i, &intersector, intervall))
    {
        triangle_hitpoint_t tmp;
        if (triangles_intersect_old(
                ray, tris, i + index_offset, intervall, &tmp))
            return true;
    }
    return false;
}

bvh_tree_t bvh_of_instance(const bvh_tree_t* bvh_of_mesh_type,
                           intervallu32_t    range)
{

    int mesh_end = range.max;
    int mesh_start = range.min;
    return (bvh_tree_t){
        .n = mesh_end - mesh_start,
        .shape_bounds = bvh_of_mesh_type->shape_bounds + mesh_start,
        .nodes = bvh_of_mesh_type->nodes + mesh_start,
        .node_bounds = bvh_of_mesh_type->node_bounds + mesh_start,
    };
}

bool instance_intersect_closest_old(const ray_trav_t*    world_ray,
                                    const geometries_t*  geom,
                                    int                  id,
                                    intervall_t          intervall,
                                    instance_hitpoint_t* result)
{

    if (id == 72)
    {
        int test = 0;
    }
    instance_t inst = geom->instances[id];
    ray_trav_t local_ray;
    // transformation
    mat4 trans, inv_trans;
    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);
    float norm = ray_transform(*world_ray, inv_trans, &local_ray);

    intervall_t local_intervall = {norm * intervall.min, norm * intervall.max};
    bool        did_intersect;
#ifdef LOG_TRAVERSAL
    printf("dist local: %f dist max: %f\n", local_intervall.max, intervall.max);
#endif
    intervallu32_t range = get_mesh_range(geom, inst);
    switch (inst.type)
    {
    case JPC_SPHERE: {
        bvh_tree_t local_tree = bvh_of_instance(geom->bvhtree_spheres, range);
        did_intersect
            = spheres_intesect_closest_old(&local_ray,
                                           &geom->spheres,
                                           local_intervall,
                                           &local_tree,
                                           range.min,
                                           &result->geom_id,
                                           &result->geom_hit.distance);

        break;
    }
    case JPC_TRIANGLE: {
        bvh_tree_t tree = bvh_of_instance(geom->bvhtree_triangles, range);
        did_intersect = triangles_intersect_closest_old(&local_ray,
                                                        &geom->triangles,
                                                        local_intervall,
                                                        &tree,
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

bool instance_intersect_closest2(const ray_trav_t*    world_ray,
                                 const geometries_t*  geom,
                                 int                  id,
                                 intervall_t          intervall,
                                 instance_hitpoint_t* result)
{

    if (id == 72)
    {
        int test = 0;
    }
    instance_t inst = geom->instances[id];
    ray_trav_t local_ray;
    // transformation
    mat4 trans, inv_trans;
    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);
    float norm = ray_transform(*world_ray, inv_trans, &local_ray);

    intervall_t local_intervall = {norm * intervall.min, norm * intervall.max};
    bool        did_intersect;
#ifdef LOG_TRAVERSAL
    printf("dist local: %f dist max: %f\n", local_intervall.max, intervall.max);
#endif
    intervallu32_t range = get_mesh_range(geom, inst);
    switch (inst.type)
    {
    case JPC_SPHERE: {
        bvh_tree_t local_tree = bvh_of_instance(geom->bvhtree_spheres, range);
        /*
        did_intersect = spheres_intesect_closest2(&local_ray,
                                                  &geom->spheres,
                                                  local_intervall,
                                                  &tree,
                                                  range.min,
                                                  &result->geom_id,
                                                  &result->geom_hit.distance);*/
        sphs_intersect_params p = {
            .hit_p = &result->geom_hit.distance,
            .index_offset = range.min,
            .out_id = &result->geom_id,
            .sps = &geom->spheres,
        };
        did_intersect = find_closest_leaf2(
            &local_tree, &local_ray, local_intervall, _sphs_intersect, &p);
        break;
    }
    case JPC_TRIANGLE: {
        bvh_tree_t tree = bvh_of_instance(geom->bvhtree_triangles, range);
        did_intersect = triangles_intersect_closest_old(&local_ray,
                                                        &geom->triangles,
                                                        local_intervall,
                                                        &tree,
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

bool instance_intersect_any(const ray_trav_t*   world_ray,
                            const geometries_t* geom,
                            int                 id,
                            intervall_t         intervall)
{

    instance_t inst = geom->instances[id];
    ray_trav_t local_ray;
    // transformation
    mat4 trans, inv_trans;
    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);
    float norm = ray_transform(*world_ray, inv_trans, &local_ray);

    intervall_t local_intervall = {norm * intervall.min, norm * intervall.max};

    intervallu32_t range = get_mesh_range(geom, inst);
    switch (inst.type)
    {
    case JPC_SPHERE: {
        bvh_tree_t tree = bvh_of_instance(geom->bvhtree_spheres, range);
        return spheres_intesect_any(
            &local_ray, &geom->spheres, intervall, &tree, range.min);
    }
    case JPC_TRIANGLE: {
        bvh_tree_t tree = bvh_of_instance(geom->bvhtree_triangles, range);
        return triangles_intersect_any(
            &local_ray, &geom->triangles, intervall, &tree, range.min);
    }
    };
    return false;
}
bool instances_intersect_closest_old(const ray_trav_t*    world_ray,
                                     const geometries_t*  geom,
                                     intervall_t          intervall,
                                     int*                 id,
                                     instance_hitpoint_t* result)
{
    bvh_intersetor_closest_t intersector;
    if (!bvh_intersect_init(
            *geom->bvhtree_instances, intervall.min, *world_ray, &intersector))
        return false;

    bool did_intersect = false;

    int i;
    while (find_closest_leaf(&i, &intersector, intervall.max))
    {
        if (instance_intersect_closest_old(
                world_ray, geom, i, intervall, result))
        {
            intervall.max = result->geom_hit.distance;
            did_intersect = true;
        };
    }
    if (did_intersect)
        *id = i;
    return did_intersect;
}

typedef struct
{
    const geometries_t*  geom;
    int*                 out_id;
    int                  offset;
    instance_hitpoint_t* out_hit;

} _inst_params;

bool _inst_intersect(void* params,

                     const ray_trav_t* world_ray,

                     intervall_t intervall,
                     int         id,
                     float*      out_max_dist)
{
    _inst_params* p = params;
    id += p->offset;
    if (instance_intersect_closest_old(
            world_ray, p->geom, id, intervall, p->out_hit))
    {
        *p->out_id = id;
        return true;
    }
    return false;
}

bool instances_intersect_closest2(const ray_trav_t*    world_ray,
                                  const geometries_t*  geom,
                                  intervall_t          intervall,
                                  int*                 id,
                                  instance_hitpoint_t* result)
{
    _inst_params p = {
        .geom = geom,
        .out_id = id,
        .out_hit = result,
    };
    return find_closest_leaf2(
        geom->bvhtree_instances, world_ray, intervall, _inst_intersect, &p);
}

bool instances_intersect_any(const ray_trav_t*   world_ray,
                             const geometries_t* geom,
                             intervall_t         intervall)
{
    bvh_intersetor_any_t intersector;
    bvh_intersect_any_init(*geom->bvhtree_instances, *world_ray, &intersector);

    int i;
    while (find_any_leaf(&i, &intersector, intervall))
    {
        if (instance_intersect_any(world_ray, geom, i, intervall))
            return true;
    }
    return false;
}

hit_point_t instances_finalize(instance_hitpoint_t hit,
                               uint                id,
                               ray_trav_t          ray,
                               geometries_t        geom)
{
    instance_t inst = geom.instances[id];
    uint* mat_slot_bindings = &geom.material_slots[inst.material_slot_start];

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
        tmp = triangles_finalize(hit.geom_hit,
                                 hit.geom_id,
                                 local_ray,
                                 geom.triangles,
                                 mat_slot_bindings);
        break;
    case JPC_SPHERE:

        tmp = spheres_finalize(hit.geom_hit.distance,
                               hit.geom_id,
                               local_ray,
                               geom.spheres,
                               mat_slot_bindings);
        break;
    }
    hit_point_t result = tmp;
    glm_mat4_mulv3(trans, tmp.location, 1, result.location);
    glm_mat4_mulv3(trans, tmp.normal, 0, result.normal);
    glm_vec3_scale(result.normal, norm, result.normal);
    return result;
}

bool ray_intersect_c(const geometries_t* geometries,
                     ray_t*              ray,
                     hit_point_t*        out_hitpoint)
{
#ifdef LOG_TRAVERSAL
    printf("--------------------------------------------\n");
#endif
    ray_trav_t          ray_trav = ray_trav_make(*ray);
    int                 id;
    instance_hitpoint_t hit
        = {.geom_hit = {.distance = 0, .uv = {0, 0}}, .geom_id = -1};
    if (!instances_intersect_closest_old(
            &ray_trav, geometries, (intervall_t){0, ray->clip_end}, &id, &hit))
        return false;

    *out_hitpoint = instances_finalize(hit, id, ray_trav, *geometries);
    return true;
}

bool ray_intersect_c2(const geometries_t* geometries,
                      ray_t*              ray,
                      hit_point_t*        out_hitpoint)
{
#ifdef LOG_TRAVERSAL
    printf("--------------------------------------------\n");
#endif
    ray_trav_t          ray_trav = ray_trav_make(*ray);
    int                 id;
    instance_hitpoint_t hit
        = {.geom_hit = {.distance = 0, .uv = {0, 0}}, .geom_id = -1};
    if (!instances_intersect_closest2(
            &ray_trav, geometries, (intervall_t){0, ray->clip_end}, &id, &hit))
        return false;

    *out_hitpoint = instances_finalize(hit, id, ray_trav, *geometries);
    return true;
}