#include "NaiveIntersection.h"
#include "../../Geometry.h"
#include "TriangleMesh.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include <optional>

namespace jpctracer::raytracing
{

template <>
IntersectionResult NaiveIntersect(AnyHitCallBack any_hit_program, const TriangleMesh& mesh, const Ray& ray,
                                  const int* material_per_slot, const Transformation& trans)
{

    std::optional<SurfaceInteraction> closest_interaction;

    for (int i = 0; i < GetSize(mesh); i++)
    {
        auto interaction = Intersect(mesh, i, ray, material_per_slot);

        if (interaction)
        {
            JPC_LOG_INFO("Triangle id: {}", i);
            interaction->Point = Apply(trans, interaction->Point);
            interaction->Normal = Apply(trans, interaction->Normal);
            AnyHitResult any_hit_result = any_hit_program(*interaction);

            if (any_hit_result.ShouldTerminate)
                return IntersectionResult{std::nullopt, true};
            if (any_hit_result.IsHit)
            {
                std::optional<SurfaceInteraction> new_interaction =
                    ClosestInteraction(closest_interaction, interaction);
                closest_interaction = new_interaction;
            }
        }
    }
    return {closest_interaction, false};
}

IntersectionResult NaiveInstancesIntersect(AnyHitCallBack any_hit_program, const Scene& scene, const Ray& ray)
{
    std::optional<SurfaceInteraction> closest_interaction;

    for (const auto& instance_trans : scene.static_instances)
    {
        const auto& instance = instance_trans.first;
        const auto& transformation = instance_trans.second;

        const int* materials_per_slot = &instance.materials_per_slot[0];

        IntersectionResult instance_result;
        Ray transformed_ray = ApplyInverse(transformation, ray);
        // JPC_LOG_INFO("Before Trans: Dir: {} Origin: {} ", ray.Direction.to_string(), ray.Origin.to_string());
        switch (instance.mesh_id.type)
        {
        case MeshTypes::TRIANGLE:
            int triangle_id = instance.mesh_id.id;
            if (triangle_id == 1)
            {
                // JPC_LOG_INFO("debug");
            }
            instance_result = NaiveIntersect(any_hit_program, scene.triangle_meshs[triangle_id], transformed_ray,
                                             materials_per_slot, transformation);
            break;
        };

        if (instance_result.ShouldTerminate)
            return {std::nullopt, true};

        closest_interaction = ClosestInteraction(closest_interaction, instance_result.interaction);
    }
    return {closest_interaction, false};
}

} // namespace jpctracer::raytracing