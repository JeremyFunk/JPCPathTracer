#pragma once
#include "bvh.h"
#include "cglm/mat4.h"
#include "cglm/vec3.h"
#include "intersect.h"
#include "jpc_api.h"

inline int left_node_idx(bvh_node_t node)
{
    return node.split_idx;
}

inline int right_node_idx(bvh_node_t node)
{
    return node.split_idx + 1;
}

inline bool is_left_leaf(bvh_node_t node)
{
    return left_node_idx(node) == node.first_idx;
}

inline bool is_right_leaf(bvh_node_t node)
{
    return right_node_idx(node) == node.last_idx;
}

typedef struct
{
    float min_distance;
    int   id;
    bool  is_leaf;
} bvh_stack_item_cl_t;

typedef struct
{
    int  id;
    bool is_leaf;
} bvh_stack_item_an_t;

bvh_stack_item_cl_t* add_to_stack_cl(int                  id,
                                     bool                 is_leaf,
                                     bvh_tree_t           tree,
                                     ray_trav_bounds_t    ray,
                                     intervall_t          search_intervall,
                                     bvh_stack_item_cl_t* stack)
{
    bounds3d_t* bounds = is_leaf ? tree.shape_bounds : tree.node_bounds;
    intervall_t hit = bounds3d_intersect(bounds[id], ray);
    if (does_intersect_intervall(hit, search_intervall))
    {
        stack->min_distance = hit.min;
        stack->is_leaf = is_leaf;
        stack->id = id;
        stack++;
        return stack;
    }
    return stack;
}

bvh_stack_item_an_t* add_to_stack_an(int                  id,
                                     bool                 is_leaf,
                                     bvh_tree_t           tree,
                                     ray_trav_bounds_t    ray,
                                     intervall_t          search_intervall,
                                     bvh_stack_item_an_t* stack)
{
    bounds3d_t* bounds = is_leaf ? tree.shape_bounds : tree.node_bounds;
    intervall_t hit = bounds3d_intersect(bounds[id], ray);
    if (does_intersect_intervall(hit, search_intervall))
    {
        stack->is_leaf = is_leaf;
        stack->id = id;
        stack++;
        return stack;
    }
    return stack;
}
bvh_stack_item_cl_t* bounds_intersect_closest(int               id,
                                              bvh_tree_t        tree,
                                              ray_trav_bounds_t ray,
                                              intervall_t search_intervall,
                                              bvh_stack_item_cl_t* stack_begin)
{
    bvh_node_t node = tree.nodes[id];

    bvh_stack_item_cl_t* stack = stack_begin;

    stack = add_to_stack_cl(left_node_idx(node),
                            is_left_leaf(node),
                            tree,
                            ray,
                            search_intervall,
                            stack);

    stack = add_to_stack_cl(right_node_idx(node),
                            is_right_leaf(node),
                            tree,
                            ray,
                            search_intervall,
                            stack);

    if (stack_begin - stack == 2
        && stack_begin[0].min_distance < stack_begin[1].min_distance)
    {
        bvh_stack_item_cl_t tmp = stack_begin[0];
        stack_begin[0] = stack_begin[1];
        stack_begin[1] = tmp;
    }
    return stack;
}

bvh_stack_item_an_t* bounds_intersect_any(int                  id,
                                          bvh_tree_t           tree,
                                          ray_trav_bounds_t    ray,
                                          intervall_t          search_intervall,
                                          bvh_stack_item_an_t* stack)
{
    bvh_node_t node = tree.nodes[id];

    stack = add_to_stack_an(left_node_idx(node),
                            is_left_leaf(node),
                            tree,
                            ray,
                            search_intervall,
                            stack);

    stack = add_to_stack_an(right_node_idx(node),
                            is_right_leaf(node),
                            tree,
                            ray,
                            search_intervall,
                            stack);
    return stack;
}

typedef struct
{
    bvh_stack_item_cl_t  stack_data[64];
    bvh_stack_item_cl_t* stack;
    bvh_stack_item_cl_t* stack_begin;
    bvh_tree_t           tree;
    float                min_distance;
    ray_trav_bounds_t    ray;
} bvh_intersetor_closest_t;

void bvh_intersect_init(bvh_tree_t                tree,
                        float                     min_distance,
                        ray_trav_t                ray,
                        bvh_intersetor_closest_t* result)
{
    result->tree = tree;
    result->stack = result->stack_data;
    result->stack_begin = result->stack_data;
    result->stack->id = 0;
    result->stack->is_leaf = tree.n == 1;
    result->stack->min_distance = min_distance + ERROR_THICKNESS;
    result->stack++;
    result->min_distance = min_distance;
    result->ray = ray_trav_bounds_make(ray);
}

bool find_closest_leaf(int*                      id,
                       bvh_intersetor_closest_t* intersector,
                       float                     max_distance)
{
    bvh_intersetor_closest_t* i = intersector;
    bvh_stack_item_cl_t       item;
    while (i->stack != i->stack_begin)
    {
        i->stack--;
        item = *i->stack;
        if (item.min_distance < max_distance)
        {
            bvh_stack_item_cl_t* stack_begin = i->stack;
            while (true)
            {
                if (item.is_leaf)
                {
                    *id = item.id;
                    return true;
                }
                i->stack = bounds_intersect_closest(
                    item.id,
                    i->tree,
                    i->ray,
                    (intervall_t){i->min_distance, max_distance},
                    i->stack);

                if (stack_begin == i->stack)
                    break;
                i->stack--;
                item = *i->stack;
            }
        };
    }
    return false;
}

typedef struct
{
    bvh_stack_item_an_t  stack_data[64];
    bvh_stack_item_an_t* stack;
    bvh_stack_item_an_t* stack_begin;
    bvh_tree_t           tree;
    ray_trav_bounds_t    ray;
} bvh_intersetor_any_t;

void bvh_intersect_any_init(bvh_tree_t            tree,
                            ray_trav_t            ray,
                            bvh_intersetor_any_t* result)
{
    result->tree = tree;
    result->stack = result->stack_data;
    result->stack_begin = result->stack_data;
    result->stack->id = 0;
    result->stack->is_leaf = tree.n == 1;
    result->stack++;
    result->ray = ray_trav_bounds_make(ray);
}

bool find_any_leaf(int*                  id,
                   bvh_intersetor_any_t* intersector,
                   intervall_t           intervall)
{
    bvh_intersetor_any_t* i = intersector;
    while (i->stack_begin != i->stack)
    {
        i->stack--;
        bvh_stack_item_an_t item = *i->stack;
        if (item.is_leaf)
        {
            *id = item.id;
            return true;
        }
        i->stack = bounds_intersect_any(
            item.id, i->tree, i->ray, intervall, i->stack);
    }
    return false;
}

bool spheres_intersect(ray_trav_t  ray,
                       spheres_t   sphs,
                       intervall_t intervall,
                       int         id,
                       float*      result)
{
    float tmp = sphere_intersect(ray, sphs.positions[id], sphs.radii[id]);
    if (does_intersect_point(tmp, intervall))
    {
        *result = tmp;
        return true;
    }
    return false;
}

bool spheres_intesect_closest(ray_trav_t  ray,
                              spheres_t   sphs,
                              intervall_t intervall,
                              bvh_tree_t  tree,
                              int*        out_id,
                              float*      out_distance)
{
    bvh_intersetor_closest_t intersector;
    float                    max_distance = intervall.max;
    bvh_intersect_init(tree, intervall.min, ray, &intersector);

    int i;
    while (find_closest_leaf(&i, &intersector, intervall.max))
    {
        spheres_intersect(ray, sphs, intervall, i, &intervall.max);
    }
    if (max_distance + ERROR_THICKNESS > intervall.max)
    {
        *out_id = i;
        *out_distance = intervall.max;
        return true;
    }
    return false;
}

bool spheres_intesect_any(ray_trav_t  ray,
                          spheres_t   sphs,
                          intervall_t intervall,
                          bvh_tree_t  tree)
{
    bvh_intersetor_any_t intersector;
    float                max_distance = intervall.max;
    bvh_intersect_any_init(tree, ray, &intersector);

    int i;
    while (find_any_leaf(&i, &intersector, intervall))
    {
        float tmp;
        if (spheres_intersect(ray, sphs, intervall, i, &tmp))
            return true;
    }
    return false;
}
bool triangles_intersect(ray_trav_t           ray,
                         triangles_t          tris,
                         int                  id,
                         intervall_t          intervall,
                         triangle_hitpoint_t* hitp)
{
    uint*               ids = tris.verticies_ids[id];
    triangle_hitpoint_t tmp = triangle_intersect2(ray,
                                                  tris.verticies[ids[0]],
                                                  tris.verticies[ids[1]],
                                                  tris.verticies[ids[2]]);
    if (does_intersect_triangle(tmp, intervall))
    {
        *hitp = tmp;
        return true;
    }
    return false;
}

bool triangles_intersect_closest(ray_trav_t           ray,
                                 triangles_t          tris,
                                 intervall_t          intervall,
                                 bvh_tree_t           tree,
                                 int*                 out_id,
                                 triangle_hitpoint_t* out_hitp)
{
    bvh_intersetor_closest_t intersector;
    bvh_intersect_init(tree, intervall.min, ray, &intersector);
    float max_distance = intervall.max;

    int i;
    while (find_closest_leaf(&i, &intersector, intervall.max))
    {
        if (triangles_intersect(ray, tris, i, intervall, out_hitp))
            intervall.max = out_hitp->distance;
    }
    if (max_distance < intervall.max)
    {
        *out_id = i;
        return true;
    }
    return false;
}

bool triangles_intersect_any(ray_trav_t  ray,
                            triangles_t tris,
                            intervall_t intervall,
                            bvh_tree_t  tree)
{
    bvh_intersetor_any_t intersector;
    float                max_distance = intervall.max;
    bvh_intersect_any_init(tree, ray, &intersector);

    int i;
    while (find_any_leaf(&i, &intersector, intervall))
    {
        triangle_hitpoint_t tmp;
        if (triangles_intersect(ray, tris, i, intervall, &tmp))
            return true;
    }
    return false;
}
typedef struct
{
    int                 geom_id;
    triangle_hitpoint_t geom_hit;
} instance_hitpoint_t;

float ray_transform(ray_trav_t ray, mat4 mat, ray_trav_t* result)
{
    glm_mat4_mulv3(mat, ray.origin, 1, result->origin);
    glm_mat4_mulv3(mat, ray.direction, 0, result->direction);
    float norm = glm_vec3_norm(result->direction);
    glm_vec3_scale(result->direction, 1. / norm, result->direction);
    return norm;
}

inline bvh_tree_t bvh_of_instance(instance_t inst,
                                  bvh_tree_t bvh_of_mesh_type,
                                  uint*      mesh_ends)
{
    int mesh_start, mesh_end;
    if (inst.mesh_id > 0)
    {
        mesh_start = mesh_ends[inst.mesh_id - 1];
    }
    else
    {
        mesh_start = 0;
    }
    mesh_end = mesh_ends[inst.mesh_id];

    return (bvh_tree_t){
        .n = mesh_end - mesh_start,
        .shape_bounds = bvh_of_mesh_type.shape_bounds + mesh_start,
        .nodes = bvh_of_mesh_type.nodes + mesh_start,
        .node_bounds = bvh_of_mesh_type.node_bounds + mesh_start,
    };
}

bool instance_intersect_closest(ray_trav_t           world_ray,
                                geometries_t         geom,
                                int                  id,
                                intervall_t          intervall,
                                instance_hitpoint_t* result)
{

    instance_t inst = geom.instances[id];
    ray_trav_t local_ray;
    // transformation
    mat4 trans, inv_trans;
    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);
    float norm = ray_transform(world_ray, inv_trans, &local_ray);

    intervall_t local_intervall = {norm * intervall.min, norm * intervall.max};
    bool        did_intersect;

    switch (inst.type)
    {
    case JPC_SPHERE: {
        bvh_tree_t tree = bvh_of_instance(
            inst, *geom.bvhtree_spheres, geom.spheres.mesh_end_idx);
        did_intersect = spheres_intesect_closest(local_ray,
                                                 geom.spheres,
                                                 intervall,
                                                 tree,
                                                 &result->geom_id,
                                                 &result->geom_hit.distance);

        break;
    }
    case JPC_TRIANGLE: {
        bvh_tree_t tree = bvh_of_instance(
            inst, *geom.bvhtree_triangles, geom.triangles.mesh_end_idx);
        did_intersect = triangles_intersect_closest(local_ray,
                                                    geom.triangles,
                                                    intervall,
                                                    tree,
                                                    &result->geom_id,
                                                    &result->geom_hit);
    }
    };
    if (did_intersect)
    {
        result->geom_hit.distance *= norm;
        return true;
    }
    return false;
}

bool instance_intersect_any(ray_trav_t   world_ray,
                            geometries_t geom,
                            int          id,
                            intervall_t  intervall )
{

    instance_t inst = geom.instances[id];
    ray_trav_t local_ray;
    // transformation
    mat4 trans, inv_trans;
    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);
    float norm = ray_transform(world_ray, inv_trans, &local_ray);

    intervall_t local_intervall = {norm * intervall.min, norm * intervall.max};

    switch (inst.type)
    {
    case JPC_SPHERE: {
        bvh_tree_t tree = bvh_of_instance(
            inst, *geom.bvhtree_spheres, geom.spheres.mesh_end_idx);
        return spheres_intesect_any(local_ray, geom.spheres, intervall, tree);
    }
    case JPC_TRIANGLE: {
        bvh_tree_t tree = bvh_of_instance(
            inst, *geom.bvhtree_triangles, geom.triangles.mesh_end_idx);
        return triangles_intersect_any(
            local_ray, geom.triangles, intervall, tree);
    }
    };
    return false;
}
bool instances_intersect_closest(ray_trav_t           world_ray,
                                 geometries_t         geom,
                                 intervall_t          intervall,
                                 int*                 id,
                                 instance_hitpoint_t* result)
{
    bvh_intersetor_closest_t intersector;
    bvh_intersect_init(
        *geom.bvhtree_instances, intervall.min, world_ray, &intersector);

    bool did_intersect = false;

    int i;
    while (find_closest_leaf(&i, &intersector, intervall.max))
    {
        did_intersect |= instance_intersect_closest(
            world_ray, geom, i, intervall, result);
    }
    if (did_intersect)
        *id = i;
    return did_intersect;
}

bool instances_intersect_any(ray_trav_t           world_ray,
                                 geometries_t         geom,
                                 intervall_t          intervall
                           )
{
    bvh_intersetor_any_t intersector;
    bvh_intersect_any_init(
        *geom.bvhtree_instances, world_ray, &intersector);

    int i;
    while (find_any_leaf(&i, &intersector, intervall))
    {
        if(instance_intersect_any(
            world_ray, geom, i, intervall))
            return true;
    }
    return false;
}

inline hit_point_t instances_finalize(instance_hitpoint_t hit,
        uint id,
        ray_trav_t ray,
        geometries_t geom)
{
    instance_t inst = geom.instances[id];
    uint* mat_slot_bindings
        = &geom.material_slots[inst.material_slot_start];
    switch (inst.type)
    {
    case JPC_TRIANGLE:
        return triangles_finalize(hit.geom_hit,
                        hit.geom_id,
                        ray,
                        geom.triangles,
                        mat_slot_bindings);
    case JPC_SPHERE:

        return spheres_finalize(hit.geom_hit.distance,
                        hit.geom_id,
                        ray,
                        geom.spheres,
                        mat_slot_bindings);
    }
    return (hit_point_t){.material_id = -1};
}
