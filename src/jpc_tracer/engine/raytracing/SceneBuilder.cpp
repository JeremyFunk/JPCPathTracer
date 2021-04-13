#include "SceneBuilder.h"
#include "detail/Scene.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"

namespace jpctracer::raytracing {
    SceneBuilder::SceneBuilder() 
    {
        
    }
    
    MaterialId SceneBuilder::AddMaterial(const void* material) 
    {
        m_scene_data->materials.push_back(material);
        m_material_count++;
        return m_material_count-1;
    }
    
    void SceneBuilder::AddHitProgramms(MaterialId material_id, RayId ray_type_id, ClosestHitFuncPtr closest_program ,AnyHitFuncPtr any_program) 
    {
        
        auto& ray_row = m_material_funcs[ray_type_id];
        while(ray_row.size()<=material_id) ray_row.push_back({nullptr,nullptr});
        ray_row[material_id]={closest_program,any_program}; 
    }

    RayId SceneBuilder::AddRayType(MissFuncPtr program) 
    {
        m_scene_data->mis_programs.push_back(program);
        int id = m_scene_data->ray_types_count;
        m_scene_data->ray_types_count++;
        return id;
         
    }
    
    MeshId SceneBuilder::AddMesh(TriangleMesh&& mesh) 
    {
        m_scene_data->triangle_meshs.push_back(std::move(mesh));
        return {MeshTypes::TRIANGLE,m_scene_data->triangle_meshs.size()-1};
    }
    
    MeshId SceneBuilder::AddMesh(SphereMesh&& mesh) 
    {
        m_scene_data->sphere_meshs.push_back(std::move(mesh));
        return {MeshTypes::SPHERE,m_scene_data->sphere_meshs.size()-1};
    }
    
    InstanceId SceneBuilder::AddInstance(MeshId mesh_id) 
    {
        m_scene_data->static_instances.push_back({Instance{mesh_id},Transformation(0)});
        return m_scene_data->static_instances.size()-1;
    }
    
    void SceneBuilder::MaterialBind(InstanceId instance, size_t material_slot, MaterialId material_id) 
    {
        auto& mat_slots = m_scene_data->static_instances[instance].first.materials_per_slot;
        while(mat_slots.size()<=material_slot) mat_slots.push_back(0);
        mat_slots[material_slot] = material_id;
    }
    
    std::unique_ptr<Scene> SceneBuilder::Build() 
    {
        size_t hit_programs_size = m_material_count*m_scene_data->ray_types_count;
        m_scene_data->closest_hit_programs.resize(hit_programs_size,nullptr);

        int ray_idx = 0;
        int mat_idx = 0;
        for(auto& ray_row : m_material_funcs)
        {
            for(auto& material_func : ray_row)
            {
                m_scene_data->closest_hit_programs[ray_idx*m_material_count+mat_idx] = material_func.first;

                m_scene_data->any_hit_programs[ray_idx*m_material_count+mat_idx] = material_func.second;
                mat_idx++;
            }
            ray_idx++;
        }
        m_scene_data->material_count = m_material_count;

    }



   


}
