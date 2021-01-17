#pragma once
#include "core/IIntegrator.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/Ray.h"
#include "core/IScene.h"
#include "core/SpectrumPasses.h"
#include "core/SurfaceProperties.h"
#include "core/constants.h"
#include <memory>

namespace integrators
{

    core::SpectrumPasses IntegrateLights(const core::Ray& ray,const core::SurfaceProperties& properties,const std::shared_ptr<core::IScene> scene,core::BsdfMemoryPtr& memory);

    struct SampleCount;

    class LightIntegrator final: public core::IIntegrator
    {
    public:
        void Init(std::shared_ptr<core::IScene> scene) final;
        std::unique_ptr<std::vector<core::Vec2>> SetupSamples(int max_sample_count) const final;
        void FillSamples(std::shared_ptr<core::ISampler> sampler, std::unique_ptr<std::vector<core::Vec2>>& data,int max_sample_count) const final;
        core::Vec2* SetStartSample(core::Vec2* samples, int sampling_idx,int max_sample_count) const final;
        core::SpectrumPasses Integrate(const core::Ray& ray,const core::Vec2* samples,core::BsdfMemoryPtr bsdf_memory) const final;

    private:
        SampleCount GetSampleCount(int max_sample_count) const;

        std::shared_ptr<core::IScene> _scene;
    };
}