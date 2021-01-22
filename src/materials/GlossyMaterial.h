#pragma once
#include "bsdfs/BeckmannDistribution.h"
#include "core/IMaterial.h"
#include "bsdfs/IFresnel.h"
#include "bsdfs/DielectricFresnel.h"
#include "bsdfs/IMicrofacetDistribution.h"
#include "core/Linalg.h"
#include <memory>
#include <color/ColorValue.h>
#include "bsdfs/CookTorranceBSDF.h"

namespace materials
{
    class GlossyMaterial final: public core::IMaterial
    {
    public:
        GlossyMaterial(std::shared_ptr<color::ColorValue> color,core::Prec roughness);
        virtual core::SpectrumPasses Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const override;
        //virtual core::IBSDF* ComputeBSDF(const core::SurfaceInteraction& interaction, core::MemoryArea& memory_area) const override;

        virtual void OverrideBSDF(core::BsdfMemoryPtr& memory, const core::SurfaceInteraction& interaction) const override;
        virtual core::BsdfMemoryInfo GetBsdfInfo() const override;
        ~GlossyMaterial();
        
    private:
        std::shared_ptr<color::ColorValue> _color;
        std::shared_ptr<core::BsdfGeneric<bsdfs::CookTorranceParams<bsdfs::DielectricFresnelParams, bsdfs::BeckmannParams>>> _bsdf;
        core::Prec _alpha;
    };

    core::Prec RoughnessToAlpha(core::Prec roughness);
}