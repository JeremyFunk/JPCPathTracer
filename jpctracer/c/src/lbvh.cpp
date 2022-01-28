#include "lbvh.hpp"
#include "traverse.hpp"
#include <limits>
bool ray_intersect2(const geometries_t* geometries,
                    ray_t*              ray,
                    hit_point_t*        out_hitpoint)
{
    ray_trav_t                ray_trav(*ray);
    lbvh_bounds3d_intersector node_intersector{ray_trav,
                                               *geometries->bvhtree_instances};

    instance_intersector leaf_intersector(ray_trav, *geometries);

    instance_hitpoint init;
    t_min(init) = std::numeric_limits<float>::infinity();
    auto               root = get_root(*geometries->bvhtree_instances);
    pair<float, float> search_intervall = {0, ray->clip_end};

    auto hitp = closest_intersect(
        root, node_intersector, leaf_intersector, search_intervall, init);
    if (t_min(hitp) < t_max(search_intervall))
    {
        *out_hitpoint = finalize(hitp, ray_trav, *geometries);
        return true;
    }
    return false;
};