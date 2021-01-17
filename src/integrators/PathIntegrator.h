#pragma once

#include "core/IIntegrator.h"
#include "core/SpectrumPasses.h"
namespace integrators
{
    class PathIntegrator : public core::IIntegrator
    {
    public:
        PathIntegrator(core::SpectrumPasses sky_color = core::SpectrumPasses::FromValue(0),int max_depth = 5);
        virtual void Init(std::shared_ptr<core::IScene> scene) override;
        virtual size_t Get2DSampleCount() override;
        virtual core::SpectrumPasses Integrate(const core::Ray& ray,const core::Vec2* samples,core::MemoryArea& memory) const override;  
    private:
        std::shared_ptr<core::IScene> _scene;
        core::SpectrumPasses _sky_color;
        int _max_depth;
    };
}