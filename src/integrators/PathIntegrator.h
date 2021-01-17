#pragma once

#include "core/IIntegrator.h"
#include "core/SpectrumPasses.h"
namespace integrators
{

    struct SampleCount;

    class PathIntegrator final: public core::IIntegrator
    {
    public:
        PathIntegrator(core::SpectrumPasses sky_color = core::SpectrumPasses::FromValue(0),int max_depth = 5);
        void Init(std::shared_ptr<core::IScene> scene) final;
        std::unique_ptr<std::vector<core::Vec2>> SetupSamples(int max_sample_count) const final;
        void FillSamples(std::shared_ptr<core::ISampler> sampler, std::unique_ptr<std::vector<core::Vec2>>& data,int max_sample_count) const final;
        core::Vec2* SetStartSample(core::Vec2* samples, int sampling_idx,int max_sample_count) const final;
        core::SpectrumPasses Integrate(const core::Ray& ray,const core::Vec2* samples,core::BsdfMemoryPtr bsdf_memory) const final;
    private:
        SampleCount GetSampleCount(int max_sample_count) const;

        std::shared_ptr<core::IScene> _scene;
        core::SpectrumPasses _sky_color;
        int _max_depth;

    };
}