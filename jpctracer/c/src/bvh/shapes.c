#include "shapes.h"
#include "cglm/call/vec3.h"
void triangles_create_bounds(const triangles_t* triangles, bounds3d_t* bounds)
{
    for (int i = 0; i < triangles->faces_count; i++)
    {
        const uint* vert_ids = triangles->verticies_ids[i];
        float* v1 = triangles->verticies[vert_ids[0]];
        float* v2 = triangles->verticies[vert_ids[1]];
        float* v3 = triangles->verticies[vert_ids[2]];

        glmc_vec3_maxv(v1, v2, bounds[i].max);
        glmc_vec3_maxv(v3, bounds[i].max, bounds[i].max);

        glmc_vec3_minv(v1, v2, bounds[i].min);
        glmc_vec3_minv(v3, bounds[i].min, bounds[i].min);
    }
}

void spheres_create_bounds(const spheres_t* spheres, bounds3d_t* bounds)
{
    for (int i = 0; i < spheres->count; i++)
    {
        glmc_vec3_adds(spheres->positions[i], -spheres->radii[i], bounds[i].min);
        glmc_vec3_adds(spheres->positions[i], spheres->radii[i], bounds[i].max);
    }
}

void triangles_get_centers(const triangles_t* triangles, vec3* centers)
{
    for (int i = 0; i < triangles->faces_count; i++)
    {

        const uint* vert_ids = triangles->verticies_ids[i];
        float* v1 = triangles->verticies[vert_ids[0]];
        float* v2 = triangles->verticies[vert_ids[1]];
        float* v3 = triangles->verticies[vert_ids[2]];

        glmc_vec3_add(v1, v2, centers[i]);
        glmc_vec3_add(v3, centers[i], centers[i]);
        glmc_vec3_scale(centers[i], 1. / 3., centers[i]);
    }
}

void spheres_get_centers(const spheres_t* spheres, vec3* centers)
{
    for (int i = 0; i < spheres->count; i++)
        glmc_vec3_copy(spheres->positions[i], centers[i]);
}
