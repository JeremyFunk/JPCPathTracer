#include "JPCRenderer.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"



namespace jpctracer::renderer
{
    void JPCRenderer::Draw(std::shared_ptr<Geometry> geomtry) 
    {
        raytracing::MeshId mesh_id;
        switch (geomtry->mesh.index()) {
            
            case 0:
                mesh_id = m_scene_builder.AddMesh(std::move(std::get<raytracing::TriangleMesh>(geomtry->mesh)));
                geomtry->mesh = mesh_id;
                break;
            case 1:
                mesh_id = m_scene_builder.AddMesh(std::move(std::get<raytracing::SphereMesh>(geomtry->mesh)));
                geomtry->mesh = mesh_id;
                break;
            case 3:
                mesh_id = std::get<raytracing::MeshId>(geomtry->mesh);
                break;
        }
        auto instance_id = m_scene_builder.AddInstance(mesh_id);
        for(auto materials: geomtry->MaterialSlots)
            m_scene_builder.MaterialBind(instance_id, materials.first, materials.second->material_id);
    }
}