#pragma once
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer
{

    struct DebugIntegrator final: public IIntegrator
    {
        void Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler,
            Tracer& tracer, film::Film& film) const;
    };

    struct DebugBehavior final: public IRayBehavior
    {
        void ClosestHitProgram(const HitPoint& hit_point, Payload* payload ,Tracer& trace) const;

        void Miss(const Spectrum& background_color, Payload* payload) const;
    };
}