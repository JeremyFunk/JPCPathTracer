#include "bvh_builder.h"
#include "../maths.h"
#include "bvh.h"
#include "cglm/mat4.h"
#include "cglm/struct/box.h"
#include "jpc_api.h"
#include "lbvh.h"
#include "shapes.h"
#include <stdlib.h>

void instances_get_bounds(const geometries_t* geometries, bounds3d_t* bounds)
{

    for (uint i = 0; i < geometries->instances_count; i++)
    {
        instance_t inst = geometries->instances[i];
        uint       mesh_id = geometries->instances[i].mesh_id;
        bounds3d_t tmp_bound;
        switch (inst.type)
        {
        case JPC_TRIANGLE:
            tmp_bound = geometries->triangles[mesh_id].bvh_tree->root_bound;
            break;
        case JPC_SPHERE:
            tmp_bound = geometries->spheres[mesh_id].bvh_tree->root_bound;
            break;
        }
        mat4 trans;
        mat4_ucopy(inst.transformations, trans);
        glm_aabb_transform(&tmp_bound.min, trans, &bounds[i].min);
    }
}

void bounds_get_centers(const bounds3d_t* bounds, uint n, vec3* centers)
{
    for (uint i = 0; i < n; i++)
    {
        bounds3d_t bound = bounds[i];
        glm_aabb_center(&bound.min, centers[i]);
    }
}

void instances_get_centers(const geometries_t* geoms,
                           const bounds3d_t*   bounds,
                           vec3*               centers)
{
    bounds_get_centers(bounds, geoms->instances_count, centers);
}

#define BVHTREE_BUILD(count, get_bounds, get_centers, params, tree)            \
    do                                                                         \
    {                                                                          \
        tree = malloc(sizeof(bvh_tree_t));                                     \
        bounds3d_t* bounds = malloc(sizeof(bounds3d_t) * (count));             \
        vec3*       centers = malloc(sizeof(bounds3d_t) * (count));            \
        get_bounds(params, bounds);                                            \
        get_centers(params, bounds, centers);                                  \
        free(bounds);                                                          \
        free(centers);                                                         \
    } while (false)

bvh_tree_t* bvhtree_triangles_build(const triangle_mesh_t* tris)
{
    bvh_tree_t* tree;
    BVHTREE_BUILD(tris->faces_count,
                  triangles_get_bounds,
                  triangles_get_centers,
                  tris,
                  tree);
    return tree;
}

bvh_tree_t* bvhtree_spheres_build(const sphere_mesh_t* spheres)
{
    bvh_tree_t* tree;
    BVHTREE_BUILD(
        spheres->count, spheres_get_bounds, spheres_get_centers, spheres, tree);
    return tree;
}
bvh_tree_t* bvhtree_instances_build(const geometries_t* geoms)
{

    bvh_tree_t* tree;
    BVHTREE_BUILD(geoms->instances_count,
                  instances_get_bounds,
                  instances_get_centers,
                  geoms,
                  tree);
    return tree;
}

void bvhtree_free(bvh_tree_t* tree)
{
    bvh_free(*tree);
    free(tree);
}
