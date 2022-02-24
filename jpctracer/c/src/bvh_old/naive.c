#include "intersect.h"
#include "jpc_api.h"
#include "log/log.h"

// updates the intervall
bool instersect_closest_spheres_naive(ray_trav_t     ray,
                                      spheres_t      sphs,
                                      intervallu32_t sphs_range,
                                      intervall_t*   intervall,
                                      hit_point_t*   out_hit,
                                      uint*          material_slot_bindings)
{
    bool did_intersect = false;
    for (int i = sphs_range.min; i < sphs_range.max; i++)
    {

        float tmp = sphere_intersect(ray, sphs.positions[i], sphs.radii[i]);
        if (does_intersect_point(tmp, *intervall))
        {
            intervall->max = tmp;
            *out_hit
                = spheres_finalize(tmp, i, ray, sphs, material_slot_bindings);
            log_sphere(sphs.positions[i], sphs.radii[i]);
            log_ray_trav(ray);
            did_intersect = true;
        }
    }
    return did_intersect;
}

bool ray_intersect_naive(const geometries_t* geometries,
                         ray_t*              ray,
                         hit_point_t*        out_hitpoint)
{
#ifdef LOG_TRAVERSAL
    printf("-------------NAIVE-------------------\n");
#endif
    intervall_t intervall = {0, ray->clip_end};
    ray_trav_t  world_ray = ray_trav_make(*ray);

    bool did_intersect = false;
    for (int i = 0; i < geometries->instances_count; i++)
    {
        instance_t inst = geometries->instances[i];
        uint*      mat_slots_bindings
            = &geometries->material_slots[inst.material_slot_start];
        ray_trav_t local_ray;
        mat4       trans, inv_trans;
        mat4_ucopy(inst.transformations, trans);
        glm_mat4_inv(trans, inv_trans);
        float norm = ray_transform(world_ray, inv_trans, &local_ray);

        intervall_t    local_intervall = transform_intervall(intervall, norm);
        intervallu32_t range = get_mesh_range(geometries, inst);
        bool           did_mesh_intersect = false;
        switch (inst.type)
        {
        case JPC_SPHERE:
            did_mesh_intersect
                = instersect_closest_spheres_naive(local_ray,
                                                   geometries->spheres,
                                                   range,
                                                   &local_intervall,
                                                   out_hitpoint,
                                                   mat_slots_bindings);
            break;
        default:
            log_warn("Instance type not covered\n");
        }
        if (did_mesh_intersect)
        {
            *out_hitpoint = transform_hitp(*out_hitpoint, trans);
            intervall = transform_intervall(local_intervall, 1. / norm);
            did_intersect = true;
        }
    }
    if (!did_intersect)
    {
    }
    return did_intersect;
}