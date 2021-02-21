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
    bool IsLightBlocked(const LightInformation& light_info, const SurfaceInteraction& interaction,const Ref<IScene> scene,
    const Vec3& interaction_point);
    
    SpectrumPasses IntegrateLights(const Ray& ray,const Vec3& interaction_point, const SurfaceInteraction& interaction,const Ref<IScene> scene,
    const BsdfMemoryPtr& bsdf);

    struct SampleCount;

    class LightIntegrator final: public IIntegrator
    {
    public:
        void Init(Ref<IScene> scene) final;
        Scope<std::vector<Vec2>> SetupSamples(int max_sample_count) const final;
        void FillSamples(Ref<ISampler> sampler, Scope<std::vector<Vec2>>& data,int max_sample_count) const final;
        Vec2* SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const final;
        SpectrumPasses Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr bsdf_memory) const final;

    private:
        SampleCount GetSampleCount(int max_sample_count) const;

        Ref<IScene> _scene;
    };
}