#include "../maths.h"
#include "jpc_api.h"
#include "log/log.h"
#include "shapes.h"

// updates the intervall
bool instersect_closest_spheres_naive(ray_t                ray,
                                      const sphere_mesh_t* sphs,
                                      hit_point_t*         out_hit)
{
    bool did_intersect = false;
    for (uint i = 0; i < sphs->count; i++)
    {
        if (sphere_intersect(&ray,
                             sphs->geometries[i].position,
                             sphs->geometries[i].radius,
                             &out_hit->distance))
        {
            did_intersect = true;
            sphere_finalize(*out_hit, &ray, sphs);
            ray.clip_end = out_hit->distance;
        }
    }
    return did_intersect;
}

bool ray_intersect_naive(const geometries_t* geometries,
                         ray_t*              world_ray,
                         hit_point_t*        out_hitpoint)
{
#ifdef LOG_TRAVERSAL
    printf("-------------NAIVE-------------------\n");
#endif

    bool did_intersect = false;
    for (uint i = 0; i < geometries->instances_count; i++)
    {
        instance_t inst = geometries->instances[i];
        ray_t      local_ray;
        mat4       trans, inv_trans;
        mat4_ucopy(inst.transformations, trans);
        glm_mat4_inv(trans, inv_trans);
        float norm = ray_transform(world_ray, inv_trans, &local_ray);

        bool did_mesh_intersect = false;
        switch (inst.type)
        {
        case JPC_SPHERE:
            did_mesh_intersect = instersect_closest_spheres_naive(
                local_ray, &geometries->spheres[i], out_hitpoint);
            break;
        default:
            log_warn("Instance type not covered\n");
        }
        if (did_mesh_intersect)
        {
            *out_hitpoint = transform_hitp(out_hitpoint, trans, 1. / norm);
            did_intersect = true;
        }
    }
    return did_intersect;
}