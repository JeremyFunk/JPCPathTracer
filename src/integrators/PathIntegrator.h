#pragma once

#include "core/IIntegrator.h"
#include "core/SpectrumPasses.h"
namespace jpc_tracer
{

    struct SampleCount;

    class PathIntegrator final: public IIntegrator
    {
    public:
        PathIntegrator(SpectrumPasses sky_color = SpectrumPasses::FromValue(0),int max_depth = 5);
        void Init(std::shared_ptr<IScene> scene) final;
        std::unique_ptr<std::vector<Vec2>> SetupSamples(int max_sample_count) const final;
        void FillSamples(std::shared_ptr<ISampler> sampler, std::unique_ptr<std::vector<Vec2>>& data,int max_sample_count) const final;
        Vec2* SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const final;
        SpectrumPasses Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr bsdf_memory) const final;
    private:
        SampleCount GetSampleCount(int max_sample_count) const;

        std::shared_ptr<IScene> _scene;
        SpectrumPasses _sky_color;
        int _max_depth;

    };
}