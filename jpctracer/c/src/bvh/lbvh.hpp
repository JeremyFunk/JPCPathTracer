#pragma once
#include "cglm/vec3.h"
extern "C"
{
#include "../utils.h"
#include "bvh.h"
#include "cglm/cglm.h"
#include "jpc_api.h"
}
#include "../types.h"
#include "intersect.hpp"
#include "traverse.hpp"
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
        nodes = tree.nodes;
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
#ifdef LOG_TRAVERSAL
        printf("Bounds: ");
        printf_arrayf(6, (float*)&left_bound);
        printf("\n");
#endif
        if (does_intersect(hit1, intervall))
        {
#ifdef LOG_TRAVERSAL
            printf("t_min: %f\nt_max: %f\n", t_min(hit1), t_max(hit1));
#endif
            *out = {
                lbvh_iterator{left_child, left_leaf}, t_min(hit1), t_max(hit1)};
            out++;
        }

#ifdef LOG_TRAVERSAL
        printf("Bounds: ");
        printf_arrayf(6, (float*)&right_bound);
#endif
        if (does_intersect(hit2, intervall))
        {
#ifdef LOG_TRAVERSAL
            printf("t_min: %f\nt_max: %f\n", t_min(hit2), t_max(hit2));
#endif
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
    int         idx_offset;
    using intervall_type = geom_hitpoint<triangle_hitpoint>;
    triangle_intersector(const ray_trav_t&  ray,
                         const triangles_t& tris,
                         int                idx_offset)
        : ray(ray), tris(tris), idx_offset(idx_offset)
    {
    }
    template <typename Intervall>
    intervall_type operator()(const lbvh_iterator&  node,
                              const Intervall&      intervall,
                              const intervall_type& init) const
    {
        int  id = node.idx + idx_offset;
        auto ids = tris.verticies_ids[id];
        auto tmp = triangle_intersect2(ray,
                                       tris.verticies[ids[0]],
                                       tris.verticies[ids[1]],
                                       tris.verticies[ids[2]]);
        if (does_intersect(tmp, intervall))
        {
            return {id, tmp};
        }
        return init;
    }
};

struct sphere_intersector
{
    ray_trav_t ray;
    spheres_t  sphs;

    int idx_offset;
    using intervall_type = geom_hitpoint<float>;
    sphere_intersector(const ray_trav_t& ray,
                       const spheres_t&  sphs,
                       int               idx_offset)
        : ray(ray), sphs(sphs), idx_offset(idx_offset)
    {
    }

    template <typename Intervall>
    intervall_type operator()(const lbvh_iterator&  node,
                              const Intervall&      intervall,
                              const intervall_type& init) const
    {

        int id = node.idx + idx_offset;
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
        printf("intervall: %f,%f\n", t_min(intervall), t_max(intervall));
#endif
        auto tmp = sphere_intersect(ray, sphs.positions[id], sphs.radii[id]);
        if (does_intersect(tmp, intervall))
        {
#ifdef LOG_TRAVERSAL
            printf("Did intersect. Distance: %f\n", tmp);
#endif
            return {id, tmp};
        }
#ifdef LOG_TRAVERSAL
        printf("Did not intersect:\n");
#endif
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
    float      norm = glm_vec3_norm(local_ray.direction);
    float      inv_norm = 1. / norm;
    glm_vec3_scale(local_ray.direction, inv_norm, local_ray.direction);
    return {local_ray, norm};
}
hit_point_t finalize(instance_hitpoint   hit,
                     const ray_trav_t&   ray,
                     const geometries_t& geoms)
{

    uint* mat_slot_bindings
        = &geoms.material_slots[hit.instance.material_slot_start];

    instance_t inst = hit.instance;
    mat4       trans, inv_trans;

    mat4_ucopy(inst.transformations, trans);
    glm_mat4_inv(trans, inv_trans);
    auto [local_ray, norm] = transform_normalized(ray, inv_trans);

    hit_point_t tmp = hit_point_t{.material_id = -1};
    t_min(hit) *= norm;
    switch (hit.instance.type)
    {
    case JPC_TRIANGLE:
        tmp = finalize(hit.geom_hitp.data,
                       hit.geom_hitp.id,
                       local_ray,
                       geoms.triangles,
                       mat_slot_bindings);
        break;
    case JPC_SPHERE:

        tmp = finalize(hit.geom_hitp.data.distance,
                       hit.geom_hitp.id,
                       local_ray,
                       geoms.spheres,
                       mat_slot_bindings);
        break;
    }

    hit_point_t result = tmp;
    glm_mat4_mulv3(trans, tmp.location, 1, result.location);
    glm_mat4_mulv3(trans, tmp.normal, 0, result.normal);
    // normalize
    glm_vec3_scale(result.normal, norm, result.normal);

    return result;
}

typedef struct
{
    uint min;
    uint max;
} intervallu32_t;
static inline intervallu32_t get_mesh_range(const spheres_t&   sps,
                                            const triangles_t& tris,
                                            instance_t         inst)
{
    uint* mesh_ends;
    switch (inst.type)
    {
    case JPC_SPHERE:
        mesh_ends = sps.mesh_end_idx;
        break;
    case JPC_TRIANGLE:
        mesh_ends = tris.mesh_end_idx;
        break;
    }
    intervallu32_t res;
    if (inst.mesh_id > 0)
    {
        res.min = mesh_ends[inst.mesh_id - 1];
    }
    else
    {
        res.min = 0;
    }
    res.max = mesh_ends[inst.mesh_id];
    return res;
}
inline bvh_tree_t bvh_of_instance(bvh_tree_t     bvh_of_mesh_type,
                                  intervallu32_t range)
{

    int mesh_end = range.max;
    int mesh_start = range.min;
    return bvh_tree_t{
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
#ifdef LOG_TRAVERSAL

        printf("world_ray: org: ");
        printf_arrayf(3, world_ray.origin);
        printf("dir: ");
        printf_arrayf(3, world_ray.direction);
        printf("\n");
        printf("local_ray: org: ");
        printf_arrayf(3, local_ray.origin);
        printf("dir: ");
        printf_arrayf(3, local_ray.direction);
        printf("\n");
#endif

        pair<float, float> local_intervall
            = {norm * t_min(intervall), norm * t_max(intervall)};

        auto local_init = init.geom_hitp;
        t_min(local_init) *= norm;

        instance_hitpoint result;

        intervallu32_t range = get_mesh_range(sphs, tris, inst);
        switch (inst.type)
        {
        case JPC_SPHERE: {
            auto leaf_intersect
                = sphere_intersector(local_ray, sphs, range.min);
            bvh_tree_t                tree = bvh_of_instance(sphs_tree, range);
            lbvh_bounds3d_intersector node_intersect(local_ray, tree);
            lbvh_iterator             root = get_root(tree);
            auto                      res = closest_intersect(root,
                                         node_intersect,
                                         leaf_intersect,
                                         local_intervall,
                                         geom_hitpoint<float>{
                                             local_init.id,
                                             local_init.data.distance,
                                         });
            result.geom_hitp.id = res.id;
            result.geom_hitp.data.distance = res.data;
            break;
        }
        case JPC_TRIANGLE: {
            auto leaf_intersect
                = triangle_intersector(local_ray, tris, range.min);
            bvh_tree_t                tree = bvh_of_instance(sphs_tree, range);
            lbvh_bounds3d_intersector node_intersect(local_ray, tree);

            lbvh_iterator root = get_root(tree);
            auto          res = closest_intersect(root,
                                         node_intersect,
                                         leaf_intersect,
                                         local_intervall,
                                         local_init);
            result.geom_hitp = res;
            break;
        }
        }
        if (t_min(result) + ERROR_THICKNESS < t_max(local_intervall))
        {
            // intersected
            t_min(result) = t_min(result) / norm;
            result.instance = inst;
        }
        else
        {
            result = init;
        }
        return result;
    }
};

bool ray_intersect_cpp(const geometries_t* geometries,
                       ray_t*              ray,
                       hit_point_t*        out_hitpoint);
