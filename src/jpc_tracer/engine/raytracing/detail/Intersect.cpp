#include "Intersect.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/BVHIntersection.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/NaiveIntersection.h"
#include <optional>

namespace jpctracer::raytracing
{
IntersectionResult Intersect(const Scene& scene, Ray& ray, AnyHitCallBack any_hit_program)
{
    if (scene.static_bvh_type == StaticBVHType::NAIVE)
    {
        return NaiveInstancesIntersect(any_hit_program, scene, ray);
    }
    else if (scene.static_bvh_type == StaticBVHType::LBVH)
    {
        // LBVH TREE
        auto intersect_func = [&scene](const Ray& ray, const int& instance_idx, AnyHitCallBack any_hit_program) {
            // Mesh intersection function
            auto& instance = scene.static_instances[instance_idx];
            auto& mesh_id = instance.first.mesh_id;
            const int* material_per_slot = &instance.first.materials_per_slot[0];
            auto& trans = instance.second;

            auto mesh_intesect_tri = [&scene, &mesh_id, material_per_slot, &trans](const Ray& ray, const int& mesh_idx,
                                                                                   AnyHitCallBack any_hit_program) {
                return IntersectMesh(ray, scene.triangle_meshs[mesh_id.id], mesh_idx, material_per_slot,
                                     any_hit_program, trans);
            };

            auto mesh_intesect_sphere = [&scene, &mesh_id, material_per_slot,
                                         &trans](const Ray& ray, const int& mesh_idx, AnyHitCallBack any_hit_program) {
                return IntersectMesh(ray, scene.sphere_meshs[mesh_id.id], mesh_idx, material_per_slot, any_hit_program,
                                     trans);
            };

            Ray mesh_ray = TransformBack(trans, ray);

            // Intersect Mesh Tree
            switch (mesh_id.type)
            {
            case MeshTypes::TRIANGLE:
                return BVHIntersect(mesh_ray, scene.static_mesh_tree[mesh_id.id], any_hit_program, mesh_intesect_tri);

            case MeshTypes::SPHERE:
                auto number_triangle_meshs = scene.triangle_meshs.size();

                return BVHIntersect(mesh_ray, scene.static_mesh_tree[mesh_id.id + number_triangle_meshs],
                                    any_hit_program, mesh_intesect_sphere);
            }
        };

        return BVHIntersect(ray, scene.static_instance_tree, any_hit_program, intersect_func);
    }

    return {std::nullopt, false};
}

} // namespace jpctracer::raytracing