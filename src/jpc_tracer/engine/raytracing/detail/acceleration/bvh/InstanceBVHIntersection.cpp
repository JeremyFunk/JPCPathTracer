#include "InstanceBVHIntersection.h"
#include "MeshBVHIntersection.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/BoundingBoxIntersection.h"
#include <optional>

namespace jpctracer::raytracing
{

IntersectionResult callMeshBVHIntersect(const int instance_idx, const Scene& scene, const Ray& ray,
                                        AnyHitCallBack any_hit_program)
{
    auto& instance = scene.static_instances[instance_idx];
    auto& mesh_info = instance.first.mesh_id;
    auto& mesh_id = mesh_info.id;
    auto& mesh_type = mesh_info.type;
    const int* material_per_slot = &instance.first.materials_per_slot[0];
    auto& trans = instance.second;

    // transform ray to instance object space
    Ray mesh_ray = TransformBack(trans, ray);

    switch (mesh_type)
    {
    case MeshTypes::TRIANGLE:
        return MeshBVHIntersect(mesh_ray, scene.static_mesh_trees[mesh_id], scene.triangle_meshs[mesh_id],
                                material_per_slot, trans, any_hit_program);

    case MeshTypes::SPHERE:
        auto number_triangle_meshs = scene.triangle_meshs.size();

        return MeshBVHIntersect(mesh_ray, scene.static_mesh_trees[mesh_id + number_triangle_meshs],
                                scene.sphere_meshs[mesh_id], material_per_slot, trans, any_hit_program);
    }
}

IntersectionResult InstanceBVHIntersect(Ray& ray, const BVHTree& tree, const Scene& scene,
                                        AnyHitCallBack any_hit_program)
{
    // empty tree
    if (tree.internal_nodes.size() == 0)
    {
        IntersectionResult interaction = callMeshBVHIntersect(0, scene, ray, any_hit_program);

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

    // stops if no more Nodes are in nodes_to_visit
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
                    IntersectionResult interaction = callMeshBVHIntersect(last_idx, scene, ray, any_hit_program);

                    if (interaction.ShouldTerminate)
                        return {std::nullopt, true};

                    if (interaction.interaction)
                    {
                        closest_interaction = ClosestInteraction(closest_interaction, interaction.interaction);
                        ray.ClipEnd = closest_interaction->Distance;
                    }
                }
            }
            else
            {
                // check next Node
                nodes_to_visit[to_visit++] = split_idx + 1;
            }

            // left leaf
            if (first_idx == split_idx)
            {
                // Bound Intersect
                if (BoundsIntersect(tree.shape_bounds[first_idx], ray, inv_dir, dir_is_negative))
                {
                    // intersect mesh
                    IntersectionResult interaction = callMeshBVHIntersect(first_idx, scene, ray, any_hit_program);

                    if (interaction.ShouldTerminate)
                        return {std::nullopt, true};

                    if (interaction.interaction)
                    {
                        closest_interaction = ClosestInteraction(closest_interaction, interaction.interaction);
                        ray.ClipEnd = closest_interaction->Distance;
                    }
                }
            }
            else
            {
                // check next Node
                nodes_to_visit[to_visit++] = split_idx;
            }
        }

        // advance
        current_idx = nodes_to_visit[--to_visit];

    } while (to_visit >= 0);

    return {closest_interaction, false};
}

} // namespace jpctracer::raytracing