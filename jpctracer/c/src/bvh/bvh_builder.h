#pragma once
#include "jpc_api.h"


bvh_tree_t* bvhtree_triangles_build(const triangle_mesh_t* tris);
bvh_tree_t* bvhtree_spheres_build(const sphere_mesh_t* spheres);
bvh_tree_t* bvhtree_instances_build(const geometries_t* geometries);

void bvhtree_free(bvh_tree_t* tree);

