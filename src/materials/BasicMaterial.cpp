#include "BasicMaterial.h"
#include "core/Bsdf.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"

#include "bsdfs/LambertianBSDF.h"
#include <memory>


namespace materials
{
    BasicMaterial::BasicMaterial(std::shared_ptr<color::ColorValue> color, core::Prec illumination=0)
        : _color(color), _illumination(illumination), _lambertian(std::make_shared<bsdfs::LambertianBSDF>())
    {
        
    }


    core::SpectrumPasses BasicMaterial::Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const 
    {
        core::Spectrum color = core::Spectrum::FromRGB(_color->GetColor(interaction.UV));
        return core::SpectrumPasses(color *_illumination);
    }
    
    void BasicMaterial::OverrideBSDF(core::BsdfMemoryPtr& memory, const core::SurfaceInteraction& interaction) const 
    {
        core::ResetBsdfMemory(memory, interaction.Normal);
        bsdfs::LambertianParams* params = _lambertian.Setup(memory,1);
        core::Spectrum color = core::Spectrum::FromRGB(_color->GetColor(interaction.UV));
        params->Reflectance = color;
    }
    
    core::BsdfMemoryInfo BasicMaterial::GetBsdfInfo() const 
    {
        core::BsdfMemoryInfo info;
        info.max_bsdf_count = 1;
        auto size = _lambertian.GetMaxSize();
        info.max_byte_size = size;
        return info;
    }
    

}