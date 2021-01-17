#include "GlossyMaterial.h"
#include "bsdfs/BSDF.h"
#include "bsdfs/BeckmannDistribution.h"
#include "bsdfs/CookTorranceBSDF.h"
#include "bsdfs/IBXDF.h"
#include "bsdfs/IFresnel.h"
#include "bsdfs/DielectricFresnel.h"
#include "core/SpectrumPasses.h"
#include <memory>


namespace materials
{
    GlossyMaterial::GlossyMaterial(std::shared_ptr<color::ColorValue> color, core::Prec roughness)
    {
        //_distribution = std::make_shared<bsdfs::BeckmannDistribution>(bsdfs::BeckmannDistribution::RoughnessToAlpha(roughness));
        _distribution = std::make_shared<bsdfs::BeckmannDistribution>(bsdfs::BeckmannDistribution::RoughnessToAlpha(roughness));
        _fresnel = std::make_shared<bsdfs::DielectricFresnel>();
        _color = color;
    }
    
    core::SpectrumPasses GlossyMaterial::Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const 
    {
        return core::SpectrumPasses::FromValue(0);
    }
    
    core::IBSDF* GlossyMaterial::ComputeBSDF(const core::SurfaceInteraction& interaction, core::MemoryArea& memory_area) const 
    {
        bsdfs::IBXDF* bxdf = (bsdfs::IBXDF*) memory_area.Create<bsdfs::CookTorranceBSDF>(_distribution.get(),_fresnel.get(), 
            core::Spectrum::FromRGB(_color->GetColor(interaction.UV)));
        return memory_area.Create<bsdfs::BSDF>(bxdf,1,interaction.Normal);
    }
}