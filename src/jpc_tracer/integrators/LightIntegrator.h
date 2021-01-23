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

namespace jpc_tracer
{

    SpectrumPasses IntegrateLights(const Ray& ray,const SurfaceProperties& properties,const std::shared_ptr<IScene> scene,BsdfMemoryPtr& memory);

    struct SampleCount;

    class LightIntegrator final: public IIntegrator
    {
    public:
        void Init(std::shared_ptr<IScene> scene) final;
        std::unique_ptr<std::vector<Vec2>> SetupSamples(int max_sample_count) const final;
        void FillSamples(std::shared_ptr<ISampler> sampler, std::unique_ptr<std::vector<Vec2>>& data,int max_sample_count) const final;
        Vec2* SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const final;
        SpectrumPasses Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr bsdf_memory) const final;

    private:
        SampleCount GetSampleCount(int max_sample_count) const;

        std::shared_ptr<IScene> _scene;
    };
}