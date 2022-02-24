#pragma once
#include "jpc_api.h"

void bvhtree_triangles_build(geometries_t* geometries);
void bvhtree_spheres_build(geometries_t* geometries);
void bvhtree_instances_build(geometries_t* geometries);

void bvhtree_triangles_free(geometries_t* geometries);
void bvhtree_spheres_free(geometries_t* geometries);
void bvhtree_instances_free(geometries_t* geometries);