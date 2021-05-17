#pragma once
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer
{
    struct DebugPayload;
    struct DebugIntegrator final: public IIntegrator
    {
        void Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler,
            Tracer& tracer, film::Film& film) const;
    };

    struct DebugBehavior final: public IRayBehavior<DebugPayload>
    {
        void ClosestHitProgram(const HitPoint& hit_point, DebugPayload* payload ,Tracer& trace) const;

        void Miss(const Spectrum& background_color, DebugPayload* payload) const;
    };
}