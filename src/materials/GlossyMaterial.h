#pragma once
#include "core/IMaterial.h"
#include "bsdfs/IFresnel.h"
#include "bsdfs/IMicrofacetDistribution.h"
#include "core/Linalg.h"
#include <memory>
#include <color/ColorValue.h>

namespace materials
{
    class GlossyMaterial : public core::IMaterial
    {
    public:
        GlossyMaterial(std::shared_ptr<color::ColorValue> color,core::Prec roughness);
        virtual core::SpectrumPasses Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const override;
        //virtual core::IBSDF* ComputeBSDF(const core::SurfaceInteraction& interaction, core::MemoryArea& memory_area) const override;

        virtual void OverrideBSDF(core::BsdfMemoryPtr& memory, const core::SurfaceInteraction& interaction) const {};

    private:
        std::shared_ptr<bsdfs::IFresnel> _fresnel;
        std::shared_ptr<bsdfs::IMicrofacetDistribution> _distribution;
        std::shared_ptr<color::ColorValue> _color;
    };
}