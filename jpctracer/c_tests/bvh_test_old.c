#include "bvh/lbvh.h"
#include "jpc_api.h"
#include "log/log.h"
#include "types.h"
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
    float     pos[] = {0, 2, 3, 4, 5};
    float     rad[] = {1, 1, 1, 1, 1};

    ray_trav_t ray = {
        .direction = {-1, 0, 0},
        .origin = {7, 0, 0},
    };
    intervall_t intervall = {0, 9};

    int   out_id;
    float out_dist;

    spheres_t sphs = sphs_create(pos, rad, n);

    bvh_tree_t* tree = bvhtree_build_spheres(sphs);

    spheres_intesect_closest(
        &ray, &sphs, intervall, tree, 0, &out_id, &out_dist);

    assert_near(out_dist, 1);
    assert(out_id == 5);

    sphs_free(sphs);

    log_debug("Test Passed");
}

void simple_instances()
{
    const int n = 6;
    float     pos[] = {0, 2, 3, 4, 5};
    float     rad[] = {1, 1, 1, 1, 1};

    ray_trav_t ray = {
        .direction = {-1, 0, 0},
        .origin = {13, 0, 0},
    };
    intervall_t intervall = {0, 20};

    // scale
    float s = 2;

    int                 out_id;
    instance_hitpoint_t out_hit;

    instance_t inst
        = {.type = JPC_SPHERE,
           .mesh_id = 0,
           .material_slot_start = 0,
           .transformations
           = {{s, 0, 0, 0}, {0, s, 0, 0}, {0, 0, s, 0}, {0, 0, 0, 1},}, };

    spheres_t sphs = sphs_create(pos, rad, n);

    bvh_tree_t* sphs_tree = bvhtree_build_spheres(sphs);

    uint         mat_slots = 0;
    geometries_t geom = {
        .instances_count = 1,
        .instances = &inst,
        .triangles.mesh_count = 0,
        .spheres = sphs,
        .material_slots = &mat_slots,
        .material_slots_count = 0,
        .bvhtree_spheres = sphs_tree,
        .bvhtree_triangles = NULL,
    };
    geom.bvhtree_instances = bvhtree_build_instances(&geom);

    instances_intersect_closest_old(&ray, &geom, intervall, &out_id, &out_hit);

    assert_near(out_hit.geom_hit.distance, 1);
    assert(out_hit.geom_id == 5);
    assert(out_id == 0);

    hit_point_t hitp;
    ray_t       ray2;
    glm_vec3_copy(ray.origin, ray2.origin);

    glm_vec3_copy(ray.direction, ray2.direction);
    ray2.clip_end = intervall.max;
    ray_intersect_c(&geom, &ray2, &hitp);
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
