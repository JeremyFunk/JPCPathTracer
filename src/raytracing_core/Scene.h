#pragma once
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

    struct Scene
    {
        void AddMaterial(int id, int parameter_size, int parameter_alignment, void(*parameter_init)(void*));
        //Closesthitprogramm:
        //void programm(const Scene* scene,const Ray& ray, Payload* input_output_payload, void* material_parameters)

        void AddClosestHitProgramm(int material_id, int ray_type_id, void(*programm)(const Scene*,const Ray&,const SurfaceInteraction&, Payload*,void*));
        //Anyhitprogramm:
        //bool programm(const Scene* scene, Payload* input_output_payload, void* material_parameters)
        //Returns wether or not the Object was hit
        void AddAnyHitProgramm(int material_id, int ray_type_id, AnyHitResult(*programm)(const Scene*,const SurfaceInteraction&,const Ray& ,Payload*,void*));

        void MissProgramm(int ray_id, void(*programm)(const Scene*,const Ray&, Payload*));

        void SetRootGeometry(RootGeometry* geometry);

        void* UserData;

        PrivateScene* m_scene_data;

    };

}