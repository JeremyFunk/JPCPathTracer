#pragma once
#include "integrators/DebugIntegrator.h"

class TestLightIntegrator final: public integrators::DebugIntegrator
{
public:
    virtual core::Vec3 PixelEffect(core::SurfaceProperties& properties,const core::Ray& ray,const core::Vec2& sample,core::BsdfMemoryPtr& memory, 
            const std::shared_ptr<core::IScene>& scene) const;

};
