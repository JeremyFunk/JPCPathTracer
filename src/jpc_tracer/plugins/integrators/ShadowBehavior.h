#pragma once
#include "jpc_tracer/engine/PluginsApi.h"

#include "Payload.h"

namespace jpctracer
{
class ShadowBehavior final : public IRayBehavior<Payload>
{
  public:
    AnyHitResult AnyHitProgram(const HitPoint& hit_point, Payload* payload) const
    {
        payload->IsShadow = true;
        return {true, true};
    }
};

} // namespace jpctracer