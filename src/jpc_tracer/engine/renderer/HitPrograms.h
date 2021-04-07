#pragma once
#include "../raytracing/Scene.h"
#include "jpc_tracer/core/MaterialType.h"

namespace jpctracer::renderer {

    template<MaterialType type, cts::RayBehavior RayBehaviorT, cts::HitPoint HitPointT>
    void ClosesHitProgram(const raytracing::Scene* scene,const Ray& ray, 
                        const SurfaceInteraction& interaction,
                        raytracing::TracingContext* context,
                        Payload* input_output_payload,
                        const void* material_settings, void* material_parameters)
    {
        HitPointT()
    }
}