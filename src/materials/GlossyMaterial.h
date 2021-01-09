#pragma once

#include "bsdfs/BeckmannDistribution.h"
#include "core/IMaterial.h"
#include "core/Linalg.h"
#include <memory>
#include "bsdfs/IFresnel.h"
namespace materials
{
    class GlossyMaterial : public core::IMaterial
    {
    public:
        GlossyMaterial(core::Spectrum color, core::Prec roughness);
        virtual core::SpectrumPasses Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const override ;
        virtual core::IBSDF* ComputeBSDF(const core::SurfaceInteraction& interaction, core::MemoryArea& memory_area) const override;
    private:
        core::Spectrum _color;
        std::shared_ptr<bsdfs::FresnelNoOp> _fresnel;
        std::shared_ptr<bsdfs::BeckmannDistribution> _distribution;
    };
}