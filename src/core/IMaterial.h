#pragma once
#include "SurfaceInteraction.h"
#include "Ray.h"
#include "Bsdf.h"
#include "core/SpectrumPasses.h"

namespace core {
    class IMaterial
    {
    public:
        virtual SpectrumPasses Illumination(const SurfaceInteraction& interaction, const Ray& ray) const = 0;
        virtual void OverrideBSDF(BsdfMemoryPtr& memory, const SurfaceInteraction& interaction) const = 0;
        virtual BsdfMemoryInfo GetBsdfInfo() const = 0;
    };
}