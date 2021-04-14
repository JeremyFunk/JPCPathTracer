#include "SceneBuilder.h"
#include "detail/Scene.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include <memory>

namespace jpctracer::raytracing {
    SceneBuilder::SceneBuilder() 
        : m_scene_data(std::make_unique<Scene>())
    {
        
    }
    


    
    
    MeshId SceneBuilder::AddMesh(TriangleMesh&& mesh) 
    {
        m_scene_data->triangle_meshs.emplace_back(mesh);
        return {MeshTypes::TRIANGLE,m_scene_data->triangle_meshs.size()-1};
    }
    
    MeshId SceneBuilder::AddMesh(SphereMesh&& mesh) 
    {
        m_scene_data->sphere_meshs.emplace_back(mesh);
        return {MeshTypes::SPHERE,m_scene_data->sphere_meshs.size()-1};
    }
    
    InstanceId SceneBuilder::AddInstance(MeshId mesh_id) 
    {
        m_scene_data->static_instances.push_back({Instance{mesh_id},Transformation(0)});
        return m_scene_data->static_instances.size()-1;
    }
    
    void SceneBuilder::MaterialBind(InstanceId instance, size_t material_slot, uint material_id) 
    {
        auto& mat_slots = m_scene_data->static_instances[instance].first.materials_per_slot;
        while(mat_slots.size()<=material_slot) mat_slots.push_back(0);
        mat_slots[material_slot] = material_id;
    }
    
    std::unique_ptr<Scene> SceneBuilder::Build(AccelerationSettings acceleration) 
    {
        
        
        m_scene_data->static_bvh_type = acceleration.StaticBVH;
        m_scene_data->dynamic_bvh_type = acceleration.DynamicBVH;
        auto scene = std::make_unique<Scene>();
        std::swap(scene,m_scene_data);
        return scene;
    }



   


}
