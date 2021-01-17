#include "BasicMaterial.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"

#include "bsdfs/LambertianBRDF.h"
#include"bsdfs/IBXDF.h"
#include"bsdfs/BSDF.h"

namespace materials
{
    BasicMaterial::BasicMaterial(std::shared_ptr<color::ColorValue> color, core::Prec illumination=0)
        : _color(color), _illumination(illumination)
    {
        
    }


    core::SpectrumPasses BasicMaterial::Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const 
    {
        core::Spectrum color = core::Spectrum::FromRGB(_color->GetColor(interaction.UV));
        return core::SpectrumPasses(color *_illumination);
    }
    
    core::IBSDF* BasicMaterial::ComputeBSDF(const core::SurfaceInteraction& interaction, core::MemoryArea& memory_area) const 
    {
        bsdfs::IBXDF* bxdf = (bsdfs::IBXDF*) memory_area.Create<bsdfs::LambertianBRDF>(core::Spectrum::FromRGB(_color->GetColor(interaction.UV)));
        return memory_area.Create<bsdfs::BSDF>(bxdf,1,interaction.Normal);
    }
}