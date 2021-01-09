#pragma once

#include "Spectrum.h"
#include "SpectrumPasses.h"
#include "Ray.h"
#include "core/IScene.h"
#include <algorithm>
#include <memory>
#include "ISampler.h"
#include "core/MemoryArea.h"

namespace core {
    class IIntegrator
    {
    public:
        virtual void Init(std::shared_ptr<IScene> scene) = 0;
        virtual SpectrumPasses Integrate(const Ray& ray,const std::shared_ptr<ISampler>& sampler,MemoryArea& memory) const = 0;
        virtual SpectrumPasses IntegrateAllLights(const SurfaceInteraction& interaction) const = 0;
    };
}