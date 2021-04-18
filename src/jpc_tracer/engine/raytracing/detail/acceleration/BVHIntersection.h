#pragma once

#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/detail/Scene.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/IntersectionInfo.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/TriangleMesh.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/SphereMesh.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/BoundingBoxIntersection.h"
#include <optional>

namespace jpctracer::raytracing
{
    template <class MeshT> // SphereMesh || TriangleMesh
    IntersectionResult IntersectMesh(const Ray& ray, const MeshT& mesh, const int& idx, const int* material_per_slot, AnyHitCallBack any_hit_program)
    {
        std::optional<SurfaceInteraction> interaction = Intersect(mesh, idx, ray, material_per_slot);

        if(interaction)
        {
            AnyHitResult any_hit_result = any_hit_program(*interaction);

                if(any_hit_result.ShouldTerminate)
                    return IntersectionResult{std::nullopt, true};
        }

        return {interaction, false};
    }

    template <class IntersectFunc>
    IntersectionResult BVHIntersect (Ray& ray, const BVHTree& tree, AnyHitCallBack any_hit_program, IntersectFunc intersect_func) // const TriangleMesh& mesh, const int* material_per_slot
    {
        if (tree.internal_nodes.size() == 0)
        {
            IntersectionResult interaction = intersect_func(ray, 0, any_hit_program);

            if (interaction.ShouldTerminate)
                return {std::nullopt,true};

            return interaction;
        }

        const Vec3 inv_dir {1/ray.Direction[0], 1/ray.Direction[1], 1/ray.Direction[2]};
        const Int3 dir_is_negative = {inv_dir[0] < 0, inv_dir[1] < 0, inv_dir[2] < 0};

        std::optional<SurfaceInteraction> closest_interaction;

        int to_visit = 0;
        int current_idx = 0;
        std::array<int, 64> nodes_to_visit;

        while(true)
        {
            if(BoundsIntersect(tree.internal_nodes[current_idx].bound, ray, inv_dir, dir_is_negative))
            {
                // Bound Intersect

                const auto& first_idx = tree.internal_nodes[current_idx].first_idx;
                const auto& last_idx = tree.internal_nodes[current_idx].last_idx;
                const auto& split_idx = tree.internal_nodes[current_idx].split_idx;

                IntersectionResult instance_result;

                //right leaf
                if(last_idx == split_idx + 1)
                {
                    // Bound Intersect
                    if(BoundsIntersect(tree.shape_bounds[last_idx], ray, inv_dir, dir_is_negative))
                    {
                        // intersect mesh
                        IntersectionResult interaction = intersect_func(ray, last_idx, any_hit_program);

                        if(interaction.ShouldTerminate) return {std::nullopt,true};

                        if(interaction.interaction)
                        {
                            closest_interaction = ClosestInteraction(closest_interaction, interaction.interaction);
                            ray.ClipEnd = closest_interaction->Distance;
                        }
                    }                    
                }
                else
                {
                    nodes_to_visit[to_visit++] = split_idx+1;
                }
             
                // left leaf
                if(first_idx == split_idx)
                {
                    // Bound Intersect
                    if(BoundsIntersect(tree.shape_bounds[first_idx], ray, inv_dir, dir_is_negative))
                    {
                        // intersect mesh
                        IntersectionResult interaction = intersect_func(ray, first_idx, any_hit_program);

                        if(interaction.ShouldTerminate) return {std::nullopt,true};

                        if(interaction.interaction)
                        {
                            closest_interaction = ClosestInteraction(closest_interaction, interaction.interaction);
                            ray.ClipEnd = closest_interaction->Distance;
                        }
                    }
                }
                else
                {
                    nodes_to_visit[to_visit++] = split_idx;
                }
            }

            if(to_visit == 0) break;
                
            current_idx = nodes_to_visit[--to_visit];
        }

        return {closest_interaction,false};
    }

    // IntersectionResult BVHStaticIntersect (Ray& ray, const Scene& scene, AnyHitCallBack any_hit_program);
}