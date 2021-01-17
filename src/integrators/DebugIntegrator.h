#pragma once
#include "core/IIntegrator.h"

namespace integrators {

    using namespace core;

    struct SampleCount;

    class DebugIntegrator : public core::IIntegrator
    {
    public:
        void Init(std::shared_ptr<IScene> scene) final;
        std::unique_ptr<std::vector<Vec2>> SetupSamples(int max_sample_count) const final;
        void FillSamples(std::shared_ptr<ISampler> sampler, std::unique_ptr<std::vector<Vec2>>& data,int max_sample_count) const final;
        core::Vec2* SetStartSample(core::Vec2* samples, int sampling_idx,int max_sample_count) const final;
        SpectrumPasses Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr memory) const final;

        virtual core::Vec3 PixelEffect(core::SurfaceProperties& properties,const core::Ray& ray,const core::Vec2& sample,BsdfMemoryPtr& memory, 
            const std::shared_ptr<core::IScene>& scene) const = 0;
    private:
        std::shared_ptr<core::IScene> _scene;

        SampleCount GetSampleCount(int max_sample_count) const;

    };
}