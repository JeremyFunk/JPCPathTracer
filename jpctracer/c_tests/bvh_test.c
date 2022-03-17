#include "bvh/bvh_builder.h"
#include "bvh/traverse.h"

#include "config.h"
#include "geom_log.h"
#include "jpc_api.h"
#include "log/log.h"
#include "types.h"
#include "utils.h"
#include <stdlib.h>

spheres_t sphs_create(float* position_x, float* radii, uint n)
{
    spheres_t r;
    r.count = n;
    r.positions = malloc(sizeof(*r.positions) * n);

    r.radii = malloc(sizeof(*r.radii) * n);
    r.material_slot_id = malloc(sizeof(*r.material_slot_id) * n);
    r.mesh_count = 1;
    r.mesh_end_idx = malloc(sizeof(*r.mesh_end_idx));
    r.mesh_end_idx[0] = n;
    for (int i = 0; i < n; i++)
    {
        r.positions[i][0] = position_x[i];
        r.positions[i][1] = 0;
        r.positions[i][2] = 0;
        r.radii[i] = radii[i];
        r.material_slot_id[i] = 0;
    }
    return r;
}

void sphs_free(spheres_t sps)
{
    free(sps.positions);
    free(sps.radii);
    free(sps.material_slot_id);
    free(sps.mesh_end_idx);
}

void simple_spheres()
{
    const int n = 6;
    float     pos[] = {0, 1, 2, 3, 4, 5};
    float     rad[] = {1, 1, 1, 1, 1, 1};

    ray_t ray = {
        .direction = {-1, ERROR_THICKNESS, ERROR_THICKNESS},
        .origin = {7, ERROR_THICKNESS, ERROR_THICKNESS},
        .clip_end = 9,
    };

    hit_point_t hit;

    spheres_t    sphs = sphs_create(pos, rad, n);
    geometries_t geoms;
    geoms.spheres = sphs;

    bvhtree_spheres_build(&geoms);
    geom_log_write("created_bounds.obj");
    bvh_log(geoms.bvhtree_spheres);
    geom_log_write("bvh sphs.obj");

    sphs_intersect_t intersect_params
        = {.positions = sphs.positions, .radii = sphs.radii, .offset = 0};

    intersect_closest(
        ray, geoms.bvhtree_spheres, spheres_intersect, &intersect_params, &hit);

    assert_near(hit.distance, 1);
    assert(hit.mesh_id == 5);
    bvhtree_spheres_free(&geoms);
    sphs_free(sphs);

    log_debug("Test Passed");
}
void simple_instances()
{
    const int n = 6;
    float     pos[] = {0, 1, 2, 3, 4, 5};
    float     rad[] = {1, 1, 1, 1, 1, 1};

    ray_t ray = {
        .direction = {-1, ERROR_THICKNESS, ERROR_THICKNESS},
        .origin = {13, ERROR_THICKNESS, ERROR_THICKNESS},
    };
    intervall_t intervall = {0, 20};

    // scale
    float s = 2;

    hit_point_t out_hit;

    instance_t inst
        = {.type = JPC_SPHERE,
           .mesh_id = 0,
           .material_slot_start = 0,
           .transformations
           = {{s, 0, 0, 0}, {0, s, 0, 0}, {0, 0, s, 0}, {0, 0, 0, 1},}, };

    spheres_t sphs = sphs_create(pos, rad, n);

    uint         mat_slots = 0;
    geometries_t geom = {
        .instances_count = 1,
        .instances = &inst,
        .triangles.mesh_count = 0,
        .spheres = sphs,
        .material_slots = &mat_slots,
        .material_slots_count = 0,
        .bvhtree_spheres = NULL,
        .bvhtree_triangles = NULL,
    };

    bvhtree_spheres_build(&geom);
    bvhtree_triangles_build(&geom);
    bvhtree_instances_build(&geom);
    bvh_log(geom.bvhtree_instances);
    geom_log_write("instance_bvh.obj");

    ray_intersect_c3(&geom, &ray, &out_hit);
    assert_near(out_hit.distance, 1);
    assert(out_hit.mesh_id == 5);

    hit_point_t hitp;
    ray_t       ray2;
    glm_vec3_copy(ray.origin, ray2.origin);

    glm_vec3_copy(ray.direction, ray2.direction);
    ray2.clip_end = intervall.max;
    ray_intersect_c3(&geom, &ray2, &hitp);
    assert_near(hitp.location[0], 12);
    assert_near(hitp.location[1], 0);
    assert_near(hitp.location[2], 0);

    assert_near(hitp.normal[0], 1);
    assert_near(hitp.normal[1], 0);
    assert_near(hitp.normal[2], 0);

    sphs_free(sphs);

    log_debug("Test Passed");
}

int main()
{
    simple_spheres();
    simple_instances();
}
