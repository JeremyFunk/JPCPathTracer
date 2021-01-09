#pragma once
#include "core/IIntegrator.h"
#include "core/Spectrum.h"
#include "core/Ray.h"
#include "core/IScene.h"
#include "core/SpectrumPasses.h"
#include <memory>

namespace integrators
{
    using namespace core;

    class PathIntegrator : public IIntegrator
    {
    public:
        PathIntegrator(int sub_sample_count=32, int max_depth=2);
        virtual void Init(std::shared_ptr<IScene> scene) override;
        virtual SpectrumPasses Integrate(const Ray& ray,const std::shared_ptr<ISampler>& sampler,MemoryArea& memory) const override;
        virtual SpectrumPasses IntegrateAllLights(const SurfaceInteraction& interaction) const override;

    private:
        std::shared_ptr<IScene> _scene;
        int _sub_sample_count;
        int _max_depth;
    };
}