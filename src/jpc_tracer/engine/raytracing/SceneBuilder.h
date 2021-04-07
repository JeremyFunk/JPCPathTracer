#pragma once
#include "Base.h"
#include "Geometry.h"
#include "SurfaceInteraction.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include <memory>

namespace jpctracer::raytracing {

    

    //Programms

    struct AnyHitResult
    {
        bool ShouldTerminate;
        bool IsHit;
    };

    struct PrivateScene;

    struct Scene;

    struct UserThreadData;
    
    using InstanceId = size_t;

    using MaterialId = size_t;
    using RayId = size_t;

    struct TracingContext;

    using ClosestHitFuncPtr = void(*)(const void* material,UserThreadData* user_data,const Ray& ray, 
                const SurfaceInteraction& interaction, Payload* input_output_payload, 
                TracingContext* context);

    using AnyHitFuncPtr = AnyHitResult(*)(const void* material, UserThreadData* user_data,const Ray& ray,
                const SurfaceInteraction& interaction,Payload* input_output_payload);

    using MissFuncPtr = void(*)(UserThreadData* user_data,const Ray&, Payload*);
    struct SceneBuilder
    {
        SceneBuilder();
        MaterialId AddMaterial(const void* material);
        
        void AddHitProgramms(MaterialId material_id, RayId ray_type_id, ClosestHitFuncPtr closest_program ,
                            AnyHitFuncPtr any_program = nullptr);

        RayId AddRayType(MissFuncPtr program);


        MeshId AddMesh(TriangleMesh&& mesh);

        MeshId AddMesh(SphereMesh&& mesh);

        InstanceId AddInstance(MeshId mesh_id);
        //InstanceId AddInstance(MeshId mesh_id, Transformation trans);
        //InstanceId AddInstance(MeshId mesh_id, Animated<Transformation> trans);
        
        

        void MaterialBind(InstanceId instance, size_t material_slot, MaterialId material_id);



        std::unique_ptr<Scene> Build();

    private:
        std::unique_ptr<Scene> m_scene_data;

        std::vector<MissFuncPtr> m_miss_funcs;
        std::vector<std::vector<std::pair<ClosestHitFuncPtr,AnyHitFuncPtr>>> m_material_funcs;

        size_t m_material_count;


    };



}