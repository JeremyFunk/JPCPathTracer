#pragma once
#include "Base.h"
#include "Geometry.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include <memory>

namespace jpctracer::raytracing {

    

    //Programms


    struct Scene;

    struct ThreadData;
    
    using InstanceId = size_t;

    using MaterialId = size_t;
    using RayId = size_t;

    struct AccelerationSettings
    {
        raytracing::DynamicBVHType DynamicBVH = raytracing::DynamicBVHType::NAIVE;
        raytracing::StaticBVHType StaticBVH = raytracing::StaticBVHType::NAIVE;
    };

    struct SceneBuilder
    {
        SceneBuilder();
        

        MeshId AddMesh(TriangleMesh&& mesh);

        MeshId AddMesh(SphereMesh&& mesh);

        InstanceId AddInstance(MeshId mesh_id);
        InstanceId AddInstance(MeshId mesh_id, Transformation trans);
        // InstanceId AddInstance(MeshId mesh_id, Animated<Transformation> trans);
        
        
        void MaterialBind(InstanceId instance, size_t material_slot, uint material_id);



        std::unique_ptr<Scene> Build(AccelerationSettings acceleration = AccelerationSettings());

    private:
        std::unique_ptr<Scene> m_scene_data;

        size_t m_material_count;

        void __BuildLBVH();
    };



}