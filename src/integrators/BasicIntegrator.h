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

    class BasicIntegrator : public IIntegrator
    {
    public:
        virtual void Init(std::shared_ptr<IScene> scene) override;
        virtual SpectrumPasses Integrate(const Ray& ray,const std::shared_ptr<ISampler>& sampler,MemoryArea& memory) const override;

    private:
        std::shared_ptr<IScene> _scene;
    };
}