#pragma once

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/IntersectionInfo.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/SphereMesh.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/TriangleMesh.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"
#include <optional>

namespace jpctracer::raytracing
{

template <class MeshT> // TriangleMesh or SphereMesh
IntersectionResult IntersectMesh(const Ray& ray, const MeshT& mesh, const int& idx, const int* material_per_slot,
                                 AnyHitCallBack any_hit_program, const Transformation& trans)
{
    // intersect mesh
    auto interaction = Intersect(mesh, idx, ray, material_per_slot);

    if (interaction)
    {
        // transform interaction back
        Vec3 ray_real_origin = TransformTo(trans, ray.Origin);

        auto temp_p = interaction->Point;
        auto temp_n = interaction->Normal;
        interaction->Point = TransformTo(trans, interaction->Point);
        interaction->Normal = TransformTo(trans, interaction->Normal);
        interaction->Distance = (interaction->Point - ray_real_origin).norm();

        // test AnyHitFunc
        AnyHitResult any_hit_result = any_hit_program(*interaction);

        if (any_hit_result.ShouldTerminate || any_hit_result.IsHit == false)
            return IntersectionResult{std::nullopt, true};

        // JPC_LOG_WARN("LBVH Intersect idx: {}, with distance: {}", idx, interaction->Distance);
    }

    return {interaction, false};
}

template <class MeshT> // TriangleMesh or SphereMesh
IntersectionResult MeshBVHIntersect(const Ray& ray, const BVHTree& tree, const MeshT& mesh,
                                    const int* material_per_slot, const Transformation& trans,
                                    AnyHitCallBack any_hit_program)
{
    // empty tree
    if (tree.internal_nodes.size() == 0)
    {
        IntersectionResult interaction = IntersectMesh(ray, mesh, 0, material_per_slot, any_hit_program, trans);

        if (interaction.ShouldTerminate)
            return {std::nullopt, true};

        return interaction;
    }

    // bound intersect acceleration
    const Vec3 inv_dir = {1 / ray.Direction[0], 1 / ray.Direction[1], 1 / ray.Direction[2]};
    const Int3 dir_is_negative = {inv_dir[0] < 0, inv_dir[1] < 0, inv_dir[2] < 0};

    std::optional<SurfaceInteraction> closest_interaction;

    // stack design to iterate
    int to_visit = 0;
    int current_idx = 0;
    std::array<int, 64> nodes_to_visit;

    do
    {
        if (BoundsIntersect(tree.internal_bounds[current_idx], ray, inv_dir, dir_is_negative))
        {
            // Bound Intersect

            const auto& first_idx = tree.internal_nodes[current_idx].first_idx;
            const auto& last_idx = tree.internal_nodes[current_idx].last_idx;
            const auto& split_idx = tree.internal_nodes[current_idx].split_idx;

            // right leaf
            if (last_idx == split_idx + 1)
            {
                // Bound Intersect
                if (BoundsIntersect(tree.shape_bounds[last_idx], ray, inv_dir, dir_is_negative))
                {
                    IntersectionResult interaction =
                        IntersectMesh(ray, mesh, last_idx, material_per_slot, any_hit_program, trans);

                    if (interaction.ShouldTerminate)
                        return {std::nullopt, true};

                    if (interaction.interaction)
                    {
                        closest_interaction = ClosestInteraction(closest_interaction, interaction.interaction);
                    }
                }
            }
            else
            {
                nodes_to_visit[to_visit++] = split_idx + 1;
            }

            // left leaf
            if (first_idx == split_idx)
            {
                // Bound Intersect
                if (BoundsIntersect(tree.shape_bounds[first_idx], ray, inv_dir, dir_is_negative))
                {
                    // intersect mesh
                    IntersectionResult interaction =
                        IntersectMesh(ray, mesh, first_idx, material_per_slot, any_hit_program, trans);

                    if (interaction.ShouldTerminate)
                        return {std::nullopt, true};

                    if (interaction.interaction)
                    {
                        closest_interaction = ClosestInteraction(closest_interaction, interaction.interaction);
                    }
                }
            }
            else
            {
                nodes_to_visit[to_visit++] = split_idx;
            }
        }

        // advance
        current_idx = nodes_to_visit[--to_visit];

    } while (to_visit >= 0);

    return {closest_interaction, false};
}

} // namespace jpctracer::raytracing