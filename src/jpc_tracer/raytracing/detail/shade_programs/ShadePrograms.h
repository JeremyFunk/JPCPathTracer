#pragma once

#include "jpc_tracer/raytracing/Base.h"
#include "jpc_tracer/raytracing/Scene.h"
#include "jpc_tracer/raytracing/SurfaceInteraction.h"
#include "jpc_tracer/raytracing/detail/shade_programs/StackParamsAllocator.h"
namespace jpctracer::raytracing
{
    struct TracingContext;
    class ShadePrograms
    {
    public:
        ShadePrograms(const Scene* scene, size_t max_stack_size, int max_material_params_size);
        void ExecuteClosestHit(const SurfaceInteraction& interaction);
        AnyHitResult ExecuteAnyHit(const SurfaceInteraction& interaction);
        void ExecuteMiss();
        void* GetMaterialParams(const SurfaceInteraction& interaction);
        void SetContext(TracingContext* context);

        void NewRay(Ray ray,Payload* payload,const AnyHitFuncPtr* anyhit_material_funcs, const ClosestHitFuncPtr* closesthit_material_funcs, 
            MissFuncPtr miss_func, const MaterialInitFuncPtr* material_init_params, const void* const * material_settings,const AllocationData* params_infos);


        ~ShadePrograms();

    private:
        const Scene* m_scene;
        Ray m_ray;

        StackParamsAllocator m_material_params;
        const AllocationData* m_params_infos;
        void* m_params_mem;
        Payload* m_payload;
        //Per Ray
        const AnyHitFuncPtr* m_anyhit_material_funcs;
        const ClosestHitFuncPtr* m_closesthit_material_funcs;
        MissFuncPtr m_miss_func;
        const MaterialInitFuncPtr* m_material_init_params;

        TracingContext* m_context;

        const void* const * m_material_settings;
        int m_last_material_id = -1;

    };
}