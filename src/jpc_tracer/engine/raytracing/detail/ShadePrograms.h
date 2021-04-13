#pragma once

#include "jpc_tracer/engine/raytracing/Base.h"
#include "Scene.h"
#include "jpc_tracer/engine/raytracing/SceneBuilder.h"
#include "jpc_tracer/engine/raytracing/SurfaceInteraction.h"

namespace jpctracer::raytracing
{
    struct TracingContext;
    class ShadePrograms
    {
    public:
        ShadePrograms(const void** materials, TracingContext* context,UserThreadData* user_data );
        void ExecuteClosestHit(const SurfaceInteraction& interaction);
        //Anyhit should always be called before ClosestHit
        AnyHitResult ExecuteAnyHit(const SurfaceInteraction& interaction);
        void ExecuteMiss();

        void SetRayType(const Ray& ray, MissFuncPtr mis_func,const AnyHitFuncPtr* any_hit_per_mat, 
                        const ClosestHitFuncPtr* closest_hit_per_mat, Payload* payload );

    private:
        TracingContext* m_context;
        const void** m_materials;

        MissFuncPtr m_mis_func;
        const AnyHitFuncPtr* m_any_hit_per_mat;
        const ClosestHitFuncPtr* m_closest_hit_per_mat;
        UserThreadData* m_user_data;
        Payload* m_payload;
        Ray m_ray;

    };

    void UpdateShadePrograms(const Scene& scene,const Ray& ray,Payload* payload, ShadePrograms& programs);
}