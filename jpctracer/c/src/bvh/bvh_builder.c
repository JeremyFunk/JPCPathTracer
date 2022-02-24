

#include "bvh_builder.h"
#include "../maths.h"
#include "bvh.h"
#include "cglm/mat4.h"
#include "cglm/struct/box.h"
#include "jpc_api.h"
#include "lbvh.h"
#include "shapes.h"
#include <stdlib.h>

bvh_tree_t* bvhtree_shapes_build(const shapes_t* shapes)

{
    bvh_tree_t* trees = malloc(sizeof(bvh_tree_t) * shapes->mesh_count);

    bounds3d_t* bounds = malloc(sizeof(bounds3d_t) * shapes->count);
    vec3*       centers = malloc(sizeof(bounds3d_t) * shapes->count);
    shapes->get_bounds(shapes->geometries, bounds);
    shapes->get_centers(shapes->geometries, bounds, centers);

    for (int i = 0; i < shapes->mesh_count; i++)
    {

        intervallu32_t range = shapes_get_range(shapes, i);
        trees[i] = lbvh_build(
            range.max - range.min, bounds + range.min, centers + range.max);
    }
    free(bounds);
    free(centers);

    return trees;
}

void bvhtree_triangles_build(geometries_t* geometries)
{
    shapes_t shapes = shapes_triangles_init(geometries);
    geometries->bvhtree_triangles = bvhtree_shapes_build(&shapes);
}
void bvhtree_spheres_build(geometries_t* geometries)
{

    shapes_t shapes = shapes_spheres_init(geometries);
    geometries->bvhtree_spheres = bvhtree_shapes_build(&shapes);
}

void instances_get_bounds(const geometries_t* geometries, bounds3d_t* bounds)
{
    assert(geometries->bvhtree_spheres != NULL);

    assert(geometries->bvhtree_triangles != NULL);

    for (int i = 0; i < geometries->instances_count; i++)
    {
        instance_t inst = geometries->instances[i];
        uint       mesh_id = geometries->instances[i].mesh_id;
        bounds3d_t tmp_bound;
        switch (inst.type)
        {
        case JPC_TRIANGLE:
            tmp_bound = geometries->bvhtree_triangles[mesh_id].root_bound;
        case JPC_SPHERE:
            tmp_bound = geometries->bvhtree_spheres[mesh_id].root_bound;
        }
        mat4 trans;
        mat4_ucopy(inst.transformations, trans);
        glm_aabb_transform(&tmp_bound.min, trans, &bounds[i].min);
    }
}

void bounds_get_centers(const bounds3d_t* bounds, uint n, vec3* centers)
{
    for (int i = 0; i < n; i++)
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

shapes_t shapes_instances_init(const geometries_t* geometries)
{
    return (shapes_t){
        .get_bounds = instances_get_bounds,
        .get_centers = instances_get_centers,
        .count = geometries->instances_count,
        .mesh_count = 1,
        .mesh_end_ids = NULL,
        .geometries = geometries,
    };
}

void bvhtree_instances_build(geometries_t* geometries)
{

    shapes_t shapes = shapes_instances_init(geometries);
    geometries->bvhtree_instances = bvhtree_shapes_build(&shapes);
}

void bvhtrees_free(bvh_tree_t* trees, uint n)
{
    for (int i = 0; i < n; i++)
    {
        bvh_free(trees[i]);
    }
}

void bvhtree_triangles_free(geometries_t* geometries)
{
    bvhtrees_free(geometries->bvhtree_triangles,
                  geometries->triangles.mesh_count);
    free(geometries->bvhtree_triangles);
}
void bvhtree_spheres_free(geometries_t* geometries)
{

    bvhtrees_free(geometries->bvhtree_spheres, geometries->spheres.mesh_count);
    free(geometries->bvhtree_spheres);
}
void bvhtree_instances_free(geometries_t* geometries)
{
    bvh_free(*geometries->bvhtree_instances);
    free(geometries->bvhtree_instances);
}