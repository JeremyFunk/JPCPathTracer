#include "BVHInetersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/BoundingBoxIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/IntersectionInfo.h"
#include <array>



namespace jpctracer::raytracing
{
    

    IntersectionResult BVHStaticIntersect(Ray& ray, const Scene& scene, AnyHitCallBack any_hit_program) 
    {
        const Vec3 inv_dir {1/ray.Direction[0], 1/ray.Direction[1], 1/ray.Direction[2]};
        const Int3 dir_is_negative = {inv_dir[0] < 0, inv_dir[1] < 0, inv_dir[2] < 0};

        int to_visit = 0;
        int current_idx = 0;
        std::array<int, 64> nodes_to_visit;

        while(true)
        {
            if(BoundsIntersect(scene.static_instance_tree.internal_nodes[current_idx].bound, ray, inv_dir, dir_is_negative))
            {
                const uint first_idx = scene.static_instance_tree.internal_nodes[current_idx].first_idx;
                const uint last_idx = scene.static_instance_tree.internal_nodes[current_idx].last_idx;
                const uint split_idx = scene.static_instance_tree.internal_nodes[current_idx].split_idx;

                //right leaf
                if(last_idx == split_idx + 1)
                {
                    // travers mesh tree
                }
                else
                {
                    nodes_to_visit[to_visit++] = split_idx+1;
                }
             
                // left leaf
                if(first_idx == split_idx)
                {
                    // travers mesh tree
                }
                else
                {
                    nodes_to_visit[to_visit++] = split_idx;
                }
            }

            if(to_visit == 0) break;
                
            current_idx = nodes_to_visit[--to_visit];
            
        }
    }

}