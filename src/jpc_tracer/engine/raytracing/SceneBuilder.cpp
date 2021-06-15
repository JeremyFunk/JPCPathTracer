#include "SceneBuilder.h"
#include "detail/Scene.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVHConstructionHelper.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BuildLBVH.h"
#include <memory>
#include <stdint.h>

namespace jpctracer::raytracing
{
SceneBuilder::SceneBuilder() : m_scene_data(std::make_unique<Scene>())
{
}

MeshId SceneBuilder::AddMesh(TriangleMesh&& mesh)
{
    m_scene_data->triangle_meshs.emplace_back(mesh);
    return {MeshTypes::TRIANGLE, m_scene_data->triangle_meshs.size() - 1};
}

MeshId SceneBuilder::AddMesh(SphereMesh&& mesh)
{
    m_scene_data->sphere_meshs.emplace_back(mesh);
    return {MeshTypes::SPHERE, m_scene_data->sphere_meshs.size() - 1};
}

InstanceId SceneBuilder::AddInstance(MeshId mesh_id)
{
    m_scene_data->static_instances.push_back({Instance{mesh_id}, Transformation::Identity()});
    return m_scene_data->static_instances.size() - 1;
}

InstanceId SceneBuilder::AddInstance(MeshId mesh_id, Transformation trans)
{
    m_scene_data->static_instances.push_back({Instance{mesh_id}, trans});
    return m_scene_data->static_instances.size() - 1;
}

void SceneBuilder::MaterialBind(InstanceId instance, size_t material_slot, uint material_id)
{
    auto& mat_slots = m_scene_data->static_instances[instance].first.materials_per_slot;
    while (mat_slots.size() <= material_slot)
        mat_slots.push_back(0);
    mat_slots[material_slot] = material_id;
}

void SceneBuilder::__BuildLBVH()
{
    JPC_LOG_INFO("Start building LBVH Trees");

    // build mesh bvhs
    m_scene_data->static_mesh_trees.reserve(m_scene_data->triangle_meshs.size() + m_scene_data->sphere_meshs.size());

    // all triangle trees
    for (auto& triangle : m_scene_data->triangle_meshs)
    {
        auto morton_codes = GenerateTriangleMortonCodes(triangle);
        // sort by morton code
        SortTriangleByMortonCode(triangle, morton_codes);

        auto bounds = GenerateTriangleBounds(triangle);

        m_scene_data->static_mesh_trees.emplace_back(BuildLBVH(std::move(bounds), std::move(morton_codes)));
    }

    // all sphere trees
    for (auto& sphere : m_scene_data->sphere_meshs)
    {
        auto morton_codes = GenerateSphereMortonCodes(sphere);
        // sort by morton code
        SortSphereByMortonCode(sphere, morton_codes);

        auto bounds = GenerateSphereBounds(sphere);

        m_scene_data->static_mesh_trees.emplace_back(BuildLBVH(std::move(bounds), std::move(morton_codes)));
    }

    JPC_LOG_INFO("Finished building Mesh LBVH Trees");

    // build instance tree
    std::vector<Bounds3D> instance_bounds;
    instance_bounds.reserve(m_scene_data->static_instances.size());

    std::vector<uint32_t> instance_morton_codes;
    instance_morton_codes.reserve(m_scene_data->static_instances.size());

    for (const auto& instance : m_scene_data->static_instances)
    {
        const auto& idx = instance.first.mesh_id.id;

        Bounds3D bound;
        if (m_scene_data->static_mesh_trees[idx].internal_nodes.size() == 0)
            bound = TransformTo(instance.second, m_scene_data->static_mesh_trees[idx].shape_bounds[0]);
        else
            bound = TransformTo(instance.second, m_scene_data->static_mesh_trees[idx].internal_bounds[0]);

        instance_bounds.emplace_back(bound);
        instance_morton_codes.emplace_back(GetBoxMortonCode(bound));
    }

    // sort by morton code
    SortVectorsByMorton(instance_morton_codes, instance_bounds, m_scene_data->static_instances);

    m_scene_data->static_instance_tree = BuildLBVH(std::move(instance_bounds), std::move(instance_morton_codes));

    JPC_LOG_INFO("Finished building Static Instance LBVH Tree");
}

std::unique_ptr<Scene> SceneBuilder::Build(AccelerationSettings acceleration)
{

    m_scene_data->static_bvh_type = acceleration.StaticBVH;
    m_scene_data->dynamic_bvh_type = acceleration.DynamicBVH;

    if (acceleration.StaticBVH == StaticBVHType::LBVH)
    {
        __BuildLBVH();
    }

    auto scene = std::make_unique<Scene>();
    std::swap(scene, m_scene_data);
    return scene;
}
} // namespace jpctracer::raytracing
