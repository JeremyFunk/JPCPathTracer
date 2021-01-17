#pragma once
#include "core/IIntegrator.h"

namespace integrators {
    //Funcs Signatur: (SurfaceProperties& , Ray&, std::shared_ptr<core::ISampler> sampler,core::MemoryArea& memory) -> Vec3
    //Vec3 is the rgb value at this point
    class DebugIntegrator : public core::IIntegrator
    {
    public:
        virtual void Init(std::shared_ptr<core::IScene> scene);
        virtual size_t Get2DSampleCount();
        virtual core::SpectrumPasses Integrate(const core::Ray& ray,const core::Vec2* samples,core::MemoryArea& memory) const;
        virtual core::Vec3 PixelEffect(core::SurfaceProperties& properties,const core::Ray& ray,const core::Vec2& sample,core::MemoryArea& memory, 
            const std::shared_ptr<core::IScene>& scene) const = 0;
    private:
        std::shared_ptr<core::IScene> _scene;

    };
}