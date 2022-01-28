#pragma once
extern "C"
{
#include "bvh.h"
#include "cglm/call/vec3.h"
#include "cglm/mat4.h"
#include "jpc_api.h"
}
#include "intersect.hpp"
#include "traverse.hpp"
#include "types.h"
#include <limits>
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

struct lbvh_iterator
{
    constexpr const static size_t max_childs_count = 2;

    int  idx;
    bool leaf;
};

bool is_leaf(const lbvh_iterator& iter)
{
    return iter.leaf;
}

struct lbvh_bounds3d_intersector
{
    vec3        origin;
    vec3        inv_dir;
    bounds3d_t* node_bounds;
    bounds3d_t* shape_bounds;
    bvh_node_t* nodes;
    lbvh_bounds3d_intersector(ray_trav_t ray, const bvh_tree_t& tree)
    {
        glm_vec3_copy(ray.origin, origin);
        for (int i = 0; i < 3; i++)
            inv_dir[i] = 1. / ray.direction[i];
        node_bounds = tree.node_bounds;
        shape_bounds = tree.shape_bounds;
    }
    template <typename Intervall, typename O>
    // I Forward Iterator with Valuetype bounds3d
    // O Output Iterator with Valuetype std::pair<float,float>
    O operator()(const lbvh_iterator& node_it,
                 const Intervall&     intervall,
                 O                    out) const
    {

        auto node = nodes[node_it.idx];

        int  left_child = left_node_idx(node);
        int  right_child = right_node_idx(node);
        bool left_leaf = is_left_leaf(node);
        bool right_leaf = is_right_leaf(node);

        const auto& left_bound
            = left_leaf ? shape_bounds[left_child] : node_bounds[left_child];

        const auto& right_bound
            = right_leaf ? shape_bounds[right_child] : node_bounds[right_child];

        auto hit1 = bounds3d_intersect(left_bound, origin, inv_dir);

        auto hit2 = bounds3d_intersect(right_bound, origin, inv_dir);

        if (does_intersect(hit1, intervall))
        {
            *out = {
                lbvh_iterator{left_child, left_leaf}, t_min(hit1), t_max(hit1)};
            out++;
        }
        if (does_intersect(hit2, intervall))
        {
            *out = {lbvh_iterator{right_child, right_leaf},
                    t_min(hit2),
                    t_max(hit2)};
            out++;
        }
        return out;
    }
};

lbvh_iterator get_root(const bvh_tree_t& tree)
{
    return {.idx = 0, .leaf = tree.n == 1};
}

template <class T> struct geom_hitpoint
{
    int  id;
    T    data;
    auto operator<=>(const geom_hitpoint& x) const
    {
        return data <=> x.data;
    }
};
template <typename T> float& t_min(geom_hitpoint<T>& x)
{
    return t_min(x.data);
}

template <typename T> float& t_max(geom_hitpoint<T>& x)
{
    return t_max(x.data);
}
template <typename T> float t_min(const geom_hitpoint<T>& x)
{
    return t_min(x.data);
}

template <typename T> float t_max(const geom_hitpoint<T>& x)
{
    return t_max(x.data);
}

struct triangle_intersector
{
    ray_trav_t  ray;
    triangles_t tris;
    using intervall_type = geom_hitpoint<triangle_hitpoint>;
    triangle_intersector(const ray_trav_t& ray, const triangles_t& tris)
        : ray(ray), tris(tris)
    {
    }
    template <typename Intervall>
    intervall_type operator()(const lbvh_iterator&  node,
                              const Intervall&      intervall,
                              const intervall_type& init) const
    {
        auto ids = tris.verticies_ids[node.idx];
        auto tmp = triangle_intersect2(ray,
                                       tris.verticies[ids[0]],
                                       tris.verticies[ids[1]],
                                       tris.verticies[ids[2]]);
        if (does_intersect(tmp, intervall))
        {
            return {node.idx, tmp};
        }
        return init;
    }
};

struct sphere_intersector
{
    ray_trav_t ray;
    spheres_t  sphs;
    using intervall_type = geom_hitpoint<float>;
    sphere_intersector(const ray_trav_t& ray, const spheres_t& sphs)
        : ray(ray), sphs(sphs)
    {
    }

    template <typename Intervall>
    intervall_type operator()(const lbvh_iterator&  node,
                              const Intervall&      intervall,
                              const intervall_type& init) const
    {
        auto tmp = sphere_intersect(
            ray, sphs.positions[node.idx], sphs.radii[node.idx]);
        if (does_intersect(tmp, intervall))
        {
            return {node.idx, tmp};
        }
        return init;
    }
};

struct instance_hitpoint
{
    // does also contain the sphere hitpoint

    geom_hitpoint<triangle_hitpoint> geom_hitp;

    instance_t instance;
};

inline float& t_min(instance_hitpoint& x)
{
    return t_min(x.geom_hitp);
}
inline float& t_max(instance_hitpoint& x)
{
    return t_max(x.geom_hitp);
}
inline float t_min(const instance_hitpoint& x)
{
    return t_min(x.geom_hitp);
}
inline float t_max(const instance_hitpoint& x)
{
    return t_min(x.geom_hitp);
}

hit_point_t finalize(const instance_hitpoint& hit,
                     const ray_trav_t&        ray,
                     const geometries_t&      geoms)
{

    uint* mat_slot_bindings
        = &geoms.material_slots[hit.instance.material_slot_start];
    switch (hit.instance.type)
    {
    case JPC_TRIANGLE:
        return finalize(hit.geom_hitp.data,
                        hit.geom_hitp.id,
                        ray,
                        geoms.triangles,
                        mat_slot_bindings);
    case JPC_SPHERE:

        return finalize(hit.geom_hitp.data.distance,
                        hit.geom_hitp.id,
                        ray,
                        geoms.spheres,
                        mat_slot_bindings);
    }
    return hit_point_t{.material_id = -1};
}

// doesnt normalizes the direction
inline ray_trav_t transform(ray_trav_t ray, mat4 mat)
{
    ray_trav_t res;
    glm_mat4_mulv3(mat, ray.origin, 1, res.origin);
    glm_mat4_mulv3(mat, ray.direction, 0, res.direction);
    return res;
}

// returns the ray and norm of the unnormalized direction
inline pair<ray_trav_t, float> transform_normalized(ray_trav_t ray, mat4 mat)
{
    ray_trav_t local_ray = transform(ray, mat);
    float      norm = glmc_vec3_norm(local_ray.direction);
    float      inv_norm = 1. / norm;
    glmc_vec3_scale(local_ray.direction, inv_norm, local_ray.direction);
    return {local_ray, norm};
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

    return {
        .n = mesh_end - mesh_start,
        .shape_bounds = bvh_of_mesh_type.shape_bounds + mesh_start,
        .nodes = bvh_of_mesh_type.nodes + mesh_start,
        .node_bounds = bvh_of_mesh_type.node_bounds + mesh_start,
    };
}

struct instance_intersector
{
    spheres_t   sphs;
    bvh_tree_t  sphs_tree;
    triangles_t tris;
    bvh_tree_t  tris_tree;

    ray_trav_t  world_ray;
    instance_t* instances;
    using intervall_type = instance_hitpoint;

    instance_intersector() = default;
    instance_intersector(ray_trav_t ray, geometries_t geometries)
        : sphs(geometries.spheres), sphs_tree(*geometries.bvhtree_spheres),
          tris(geometries.triangles), tris_tree(*geometries.bvhtree_triangles),
          world_ray(ray), instances(geometries.instances)

    {
    }

    template <class Intervall>
    instance_hitpoint operator()(const lbvh_iterator& node,
                                 const Intervall&     intervall,
                                 instance_hitpoint    init) const
    {
        instance_t inst = instances[node.idx];
        // transformation
        mat4 trans, inv_trans;
        mat4_ucopy(inst.transformations, trans);
        glm_mat4_inv(trans, inv_trans);
        auto [local_ray, norm] = transform_normalized(world_ray, inv_trans);

        pair<float, float> local_intervall
            = {norm * t_min(intervall), norm * t_max(intervall)};

        instance_hitpoint result;

        switch (inst.type)
        {
        case JPC_SPHERE: {
            auto       leaf_intersect = sphere_intersector(local_ray, sphs);
            bvh_tree_t tree
                = bvh_of_instance(inst, sphs_tree, sphs.mesh_end_idx);
            lbvh_bounds3d_intersector node_intersect(local_ray, tree);
            lbvh_iterator             root = get_root(tree);
            auto                      res = closest_intersect(root,
                                         node_intersect,
                                         leaf_intersect,
                                         local_intervall,
                                         geom_hitpoint<float>{
                                             init.geom_hitp.id,
                                             init.geom_hitp.data.distance,
                                         });
            result.geom_hitp.id = res.id;
            result.geom_hitp.data.distance = res.data;
            break;
        }
        case JPC_TRIANGLE: {
            auto       leaf_intersect = triangle_intersector(local_ray, tris);
            bvh_tree_t tree
                = bvh_of_instance(inst, tris_tree, tris.mesh_end_idx);
            lbvh_bounds3d_intersector node_intersect(local_ray, tree);

            lbvh_iterator root = get_root(tree);
            auto          res = closest_intersect(root,
                                         node_intersect,
                                         leaf_intersect,
                                         local_intervall,
                                         init.geom_hitp);
            result.geom_hitp = res;
            break;
        }
        }
        if (t_min(result) < t_min(local_intervall))
        {
            // intersected
            t_min(result) = t_min(result) / norm;
        }
        else
        {
            result = init;
        }
        return result;
    }
};
