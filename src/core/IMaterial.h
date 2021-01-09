#pragma once
#include "SurfaceInteraction.h"
#include "Ray.h"
#include "IBSDF.h"
#include "core/MemoryArea.h"
#include "core/SpectrumPasses.h"

namespace core {
    class IMaterial
    {
    public:
        virtual SpectrumPasses Illumination(const SurfaceInteraction& interaction, const Ray& ray) const = 0;
        virtual IBSDF* ComputeBSDF(const SurfaceInteraction& interaction, MemoryArea& memory_area) const = 0;  
    };
}