#include "NaiveIntersection.h"
#include "../../Geometry.h"
#include "SphereMesh.h"
#include "TriangleMesh.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include <optional>

namespace jpctracer::raytracing
{

template <class T>
IntersectionResult NaiveIntersect(AnyHitCallBack any_hit_program, const T& mesh, const Ray& ray,
                                  const int* material_per_slot, const Transformation& trans)
{

    std::optional<SurfaceInteraction> closest_interaction;

    Vec3 ray_real_origin = TransformTo(trans, ray.Origin);

    for (int i = 0; i < GetSize(mesh); i++)
    {
        auto interaction = Intersect(mesh, i, ray, material_per_slot);

        if (interaction)
        {
            auto temp_p = interaction->Point;
            auto temp_n = interaction->Normal;
            interaction->Point = TransformTo(trans, interaction->Point);
            interaction->Normal = TransformTo(trans, interaction->Normal);
            interaction->Distance = (interaction->Point - ray_real_origin).norm();
            AnyHitResult any_hit_result = any_hit_program(*interaction);

            if (any_hit_result.ShouldTerminate)
                return IntersectionResult{std::nullopt, true};
            if (any_hit_result.IsHit)
            {
                std::optional<SurfaceInteraction> new_interaction =
                    ClosestInteraction(closest_interaction, interaction);
                closest_interaction = new_interaction;
            }

            // JPC_LOG_WARN("Naive Intersect idx: {}  with distance: {}\n", i, interaction->Distance);
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
        Ray transformed_ray = TransformBack(transformation, ray);
        // JPC_LOG_INFO("Before Trans: Dir: {} Origin: {} ", ray.Direction.to_string(), ray.Origin.to_string());
        int mesh_id = instance.mesh_id.id;
        switch (instance.mesh_id.type)
        {

        case MeshTypes::TRIANGLE:

            instance_result = NaiveIntersect(any_hit_program, scene.triangle_meshs[mesh_id], transformed_ray,
                                             materials_per_slot, transformation);
            break;
        case MeshTypes::SPHERE:
            instance_result = NaiveIntersect(any_hit_program, scene.sphere_meshs[mesh_id], transformed_ray,
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