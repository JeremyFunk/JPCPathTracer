#include "GlossyMaterial.h"
#include "bsdfs/BeckmannDistribution.h"
#include "bsdfs/IBXDF.h"
#include "bsdfs/TorranceSparrowBRDF.h"
#include "core/Spectrum.h"
#include "bsdfs/BSDF.h"
#include <memory>


namespace materials
{
    GlossyMaterial::GlossyMaterial(core::Spectrum color, core::Prec roughness) 
        : _color(color),_distribution(std::make_shared<bsdfs::BeckmannDistribution>(roughness))
    {
        _fresnel = std::make_shared<bsdfs::FresnelNoOp>();
    }

    core::SpectrumPasses GlossyMaterial::Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const
    {
        return core::SpectrumPasses(core::Spectrum::FromValue(0));
    }


    
    core::IBSDF* GlossyMaterial::ComputeBSDF(const core::SurfaceInteraction& interaction, core::MemoryArea& memory_area) const
    {
        bsdfs::IBXDF* bxdf = (bsdfs::IBXDF*) memory_area.Create<bsdfs::TorranceSparrowBRDF>(_color,_fresnel.get(),_distribution.get());
        return memory_area.Create<bsdfs::BSDF>(bxdf,1,interaction.Normal);
    }
}