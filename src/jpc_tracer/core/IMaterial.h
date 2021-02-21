#pragma once
#include"Base.h"
#include "SurfaceInteraction.h"
#include "Ray.h"
#include "Bsdf.h"
#include "core/SpectrumPasses.h"

namespace jpc_tracer {
    class IMaterial
    {
    public:
        virtual SpectrumPasses Illumination(const SurfaceInteraction& interaction, const Ray& ray) const = 0;
        //scattering_direction
        virtual void OverrideBSDF(BsdfMemoryPtr& memory, const SurfaceInteraction& interaction) const = 0;
        virtual BsdfMemoryInfo GetBsdfInfo() const = 0;
    };

    
}