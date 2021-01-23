#pragma once
#include "core/IIntegrator.h"

namespace jpc_tracer {

    struct SampleCount;

    class DebugBsdfIntegrator : public IIntegrator
    {
    public:
        DebugBsdfIntegrator(int max_depth);
        void Init(Ref<IScene> scene) final;
        Scope<std::vector<Vec2>> SetupSamples(int max_sample_count) const final;
        void FillSamples(Ref<ISampler> sampler, Scope<std::vector<Vec2>>& data,int max_sample_count) const final;
        Vec2* SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const final;
        SpectrumPasses Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr memory) const final;

        virtual Vec3 PixelEffect(SurfaceProperties& properties,const Ray& ray,const Vec2& sample,BsdfMemoryPtr& bsdf_memory, 
            const Ref<IScene>& scene) const = 0;
    private:
        Ref<IScene> _scene;

        SampleCount GetSampleCount(int max_sample_count) const;

        int _max_depth;

    };
}