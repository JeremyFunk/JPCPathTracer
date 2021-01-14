#pragma once
#include "core/IIntegrator.h"
#include "core/Spectrum.h"
#include "core/Ray.h"
#include "core/IScene.h"
#include "core/SpectrumPasses.h"
#include "core/SurfaceProperties.h"
#include "textures/Texture.h"
#include "utilities/textureLoader.h"
#include "core/constants.h"
#include <memory>

namespace integrators
{

    core::SpectrumPasses IntegrateLights(const core::Ray& ray,const core::SurfaceProperties& properties,const std::shared_ptr<core::IScene> scene,core::IBSDF* bsdf);

    class LightIntegrator : public core::IIntegrator
    {
    public:
        virtual void Init(std::shared_ptr<core::IScene> scene) override;
        virtual core::SpectrumPasses Integrate(const core::Ray& ray,const std::shared_ptr<core::ISampler>& sampler,core::MemoryArea& memory) const override;

    private:
        std::shared_ptr<core::IScene> _scene;
    };
}