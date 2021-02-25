#pragma once
#include "Base.h"
#include "Geometry.h"
#include "SurfaceInteraction.h"

namespace jpc_rt_core {

    

    //Programms

    struct AnyHitResult
    {
        bool ShouldTerminate;
        bool IsHit;
    };

    struct PrivateScene;

    struct Scene;

    struct AllocationData final
    {
        size_t Size;
        size_t Alignment;
    };

    
    struct TracingContext;

    using ClosestHitFuncPtr = void(*)(const Scene* scene,const Ray& ray, const SurfaceInteraction& interaction,TracingContext* context,
                                    Payload* input_output_payload,const void* material_settings, void* material_parameters);

    using AnyHitFuncPtr = AnyHitResult(*)(const Scene* scene,const Ray& ray,const SurfaceInteraction& interaction,
                                    Payload* input_output_payload,const void* material_settings,void* material_parameters);

    using MissFuncPtr = void(*)(const Scene*,const Ray&, Payload*);

    using MaterialInitFuncPtr = void(*)(const Scene* scene,const Ray& ray,const SurfaceInteraction& interaction,const void* material_settings, void* material_parameters);

    struct Scene
    {
        Scene(void* user_data);

        void AddMaterial(int material_id, int ray_type_id,AllocationData parameter_info, MaterialInitFuncPtr parameter_init ,const void* settings);
        
        void AddClosestHitProgramm(int material_id, int ray_type_id, ClosestHitFuncPtr program );
        
        void AddAnyHitProgramm(int material_id, int ray_type_id, AnyHitFuncPtr program);

        void MissProgramm(int ray_id, MissFuncPtr program);

        RootGeometry* CreateRootGeometry(InstanceBVHType instance_bvh, DynamicBVHType dynamic_bvh, StaticBVHType static_bvh);

        void* UserData;

        PrivateScene* m_scene_data;

        ~Scene();

    };

}