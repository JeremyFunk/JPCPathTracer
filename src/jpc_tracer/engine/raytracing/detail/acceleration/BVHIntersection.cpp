#include "BVHIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/IntersectionInfo.h"

namespace jpctracer::raytracing
{
template <class MeshT> // SphereMesh || TriangleMesh
IntersectionResult IntersectMesh(const Ray& ray, const MeshT& mesh, const int& idx, const int* material_per_slot,
                                 AnyHitCallBack any_hit_program, const Transformation& trans)
{
    auto interaction = Intersect(mesh, idx, ray, material_per_slot);

    Vec3 ray_real_origin = Apply(trans, ray.Origin);

    if (interaction)
    {
        auto temp_p = interaction->Point;
        auto temp_n = interaction->Normal;
        interaction->Point = Apply(trans, interaction->Point);
        interaction->Normal = Apply(trans, interaction->Normal);
        interaction->Distance = (interaction->Point - ray_real_origin).norm();
        AnyHitResult any_hit_result = any_hit_program(*interaction);

        if (any_hit_result.ShouldTerminate)
            return IntersectionResult{std::nullopt, true};
    }

    return {interaction, false};
}

template <class IntersectFunc>
IntersectionResult BVHTraversal(Ray& ray, const BVHTree& tree, AnyHitCallBack any_hit_program,
                                IntersectFunc intersect_func)
{
    if (tree.internal_nodes.size() == 0)
    {
        IntersectionResult interaction = intersect_func(ray, 0, any_hit_program);

        if (interaction.ShouldTerminate)
            return {std::nullopt, true};

        return interaction;
    }

    const Vec3 inv_dir{1 / ray.Direction[0], 1 / ray.Direction[1], 1 / ray.Direction[2]};
    const Int3 dir_is_negative = {inv_dir[0] < 0, inv_dir[1] < 0, inv_dir[2] < 0};

    std::optional<SurfaceInteraction> closest_interaction;

    int to_visit = 0;
    int current_idx = 0;
    std::array<int, 64> nodes_to_visit;

    while (true)
    {
        if (BoundsIntersect(tree.internal_nodes[current_idx].bound, ray, inv_dir, dir_is_negative))
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
                    // intersect mesh
                    IntersectionResult interaction = intersect_func(ray, last_idx, any_hit_program);

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
                nodes_to_visit[to_visit++] = split_idx + 1;
            }

            // left leaf
            if (first_idx == split_idx)
            {
                // Bound Intersect
                if (BoundsIntersect(tree.shape_bounds[first_idx], ray, inv_dir, dir_is_negative))
                {
                    // intersect mesh
                    IntersectionResult interaction = intersect_func(ray, first_idx, any_hit_program);

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
                nodes_to_visit[to_visit++] = split_idx;
            }
        }

        if (to_visit == 0)
            break;

        current_idx = nodes_to_visit[--to_visit];
    }

    return {closest_interaction, false};
}

IntersectionResult MeshBVHIntersect(const Scene& scene, Ray& ray, const int& instance_idx,
                                    AnyHitCallBack any_hit_program)
{
    auto& instance = scene.static_instances[instance_idx];
    auto& mesh_id = instance.first.mesh_id;
    const int* material_per_slot = &instance.first.materials_per_slot[0];
    auto& trans = instance.second;

    // transform ray to instance object space
    Ray mesh_ray = ApplyInverse(trans, ray);

    // intersect mesh geometry
    auto mesh_intesect_tri = [&scene, &mesh_id, material_per_slot, &trans](const Ray& ray, const int& mesh_idx,
                                                                           AnyHitCallBack any_hit_program) {
        return IntersectMesh(ray, scene.triangle_meshs[mesh_id.id], mesh_idx, material_per_slot, any_hit_program,
                             trans);
    };

    auto mesh_intesect_sphere = [&scene, &mesh_id, material_per_slot, &trans](const Ray& ray, const int& mesh_idx,
                                                                              AnyHitCallBack any_hit_program) {
        return IntersectMesh(ray, scene.sphere_meshs[mesh_id.id], mesh_idx, material_per_slot, any_hit_program, trans);
    };

    // Intersect Mesh Tree
    switch (mesh_id.type)
    {
    case MeshTypes::TRIANGLE:
        return BVHTraversal(mesh_ray, scene.static_mesh_trees[mesh_id.id], any_hit_program, mesh_intesect_tri);

    case MeshTypes::SPHERE:
        auto number_triangle_meshs = scene.triangle_meshs.size();

        return BVHTraversal(mesh_ray, scene.static_mesh_trees[mesh_id.id + number_triangle_meshs], any_hit_program,
                            mesh_intesect_sphere);
    }
}

IntersectionResult BVHIntersect(Ray& ray, const Scene& scene, AnyHitCallBack any_hit_program)
{
    // LBVH TREE
    auto mesh_tree_intersect = [&scene](Ray& ray, const int& instance_idx, AnyHitCallBack any_hit_program) {
        return MeshBVHIntersect(scene, ray, instance_idx, any_hit_program);
    };

    return BVHTraversal(ray, scene.static_instance_tree, any_hit_program, mesh_tree_intersect);
}
} // namespace jpctracer::raytracing