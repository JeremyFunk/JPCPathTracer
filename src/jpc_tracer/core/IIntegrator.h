#pragma once

#include "Spectrum.h"
#include "SpectrumPasses.h"
#include "Ray.h"
#include "core/IScene.h"
#include <algorithm>
#include <memory>
#include <vector>
#include "ISampler.h"
#include "core/Linalg.h"
#include "Bsdf.h"

namespace jpc_tracer {
    class IIntegrator
    {
    public:
        virtual void Init(std::shared_ptr<IScene> scene) = 0;
        virtual std::unique_ptr<std::vector<Vec2>> SetupSamples(int max_sample_count) const = 0;
        virtual void FillSamples(std::shared_ptr<ISampler> sampler, std::unique_ptr<std::vector<Vec2>>& data,int max_sample_count) const = 0;
        virtual Vec2* SetStartSample(Vec2* samples, int sampling_idx,int max_sample_count) const = 0;
        virtual SpectrumPasses Integrate(const Ray& ray,const Vec2* samples,BsdfMemoryPtr bsdf_memory) const = 0;
    };
}