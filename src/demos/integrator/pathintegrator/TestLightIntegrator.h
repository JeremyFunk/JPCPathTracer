#pragma once
#include "integrators/DebugIntegrator.h"

namespace jpc_tracer {

class TestLightIntegrator final: public DebugIntegrator
{
public:
    virtual Vec3 PixelEffect(SurfaceProperties& properties,const Ray& ray,const Vec2& sample,BsdfMemoryPtr& memory, 
            const std::shared_ptr<IScene>& scene) const;

};
}
