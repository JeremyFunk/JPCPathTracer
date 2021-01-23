#pragma once
#include "core/IIntegrator.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/Ray.h"
#include "core/IScene.h"
#include "core/SpectrumPasses.h"
#include <memory>

namespace jpc_tracer
{
    class BasicIntegrator final : public IIntegrator
    {
    public:
        void Init(Ref<IScene> scene) override;
        Scope<std::vector<Vec2>> SetupSamples(int max_sample_count) const override;
        void FillSamples(Ref<ISampler> sampler, Scope<std::vector<Vec2>>& data,int max_sample_count) const override;
        Vec2* SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const override;
        SpectrumPasses Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr memory) const override;

    private:
        Ref<IScene> _scene;
    };
}