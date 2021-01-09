#pragma once
#include "core/IIntegrator.h"
#include "core/Spectrum.h"
#include "core/Ray.h"
#include "core/IScene.h"
#include "core/SpectrumPasses.h"
#include "textures/Texture.h"
#include "utilities/textureLoader.h"
#include "core/constants.h"
#include <memory>

namespace integrators
{
    using namespace core;

    class LightIntegrator : public IIntegrator
    {
    public:
        virtual void Init(std::shared_ptr<IScene> scene) override;
        virtual SpectrumPasses Integrate(const Ray& ray,const std::shared_ptr<ISampler>& sampler,MemoryArea& memory) const override;
        virtual SpectrumPasses IntegrateAllLights(const SurfaceInteraction& interaction) const override;

    private:
        std::shared_ptr<IScene> _scene;
        std::shared_ptr<textures::Texture> texture;
    };
}