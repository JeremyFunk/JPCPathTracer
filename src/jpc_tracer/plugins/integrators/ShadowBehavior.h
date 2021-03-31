#pragma once
#include "jpc_tracer/core/Concepts.h"
#include "jpc_tracer/core/archetypes.h"
#include"jpc_tracer/core/core.h"
#include "Payload.h"
namespace jpctracer
{
    class ShadowBehavior
    {
    public:
        AnyHitResult AnyHitProgram(const cts::HitPoint auto& hit_point, Payload* payload)
        {
            payload->IsShadow = true;
            return {true,true};
        }    
    };
    static_assert(cts::HitPoint<archetypes::HitPoint>);
    static_assert(cts::RayBehavior<ShadowBehavior>);


}