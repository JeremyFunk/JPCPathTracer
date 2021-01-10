#pragma once
#include "core/IMaterial.h"
#include "bsdfs/IFresnel.h"
#include "bsdfs/IMicrofacetDistribution.h"
#include "core/Linalg.h"
#include <memory>

namespace materials
{
    class GlossyMaterial : public core::IMaterial
    {
    public:
        GlossyMaterial(core::Prec roughness);
        virtual core::SpectrumPasses Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const override;
        virtual core::IBSDF* ComputeBSDF(const core::SurfaceInteraction& interaction, core::MemoryArea& memory_area) const override;

    private:
        std::shared_ptr<bsdfs::IFresnel> _fresnel;
        std::shared_ptr<bsdfs::IMicrofacetDistribution> _distribution;
    };
}