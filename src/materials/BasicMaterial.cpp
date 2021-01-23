#include "BasicMaterial.h"
#include "core/Bsdf.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"

#include "bsdfs/LambertianBSDF.h"
#include <memory>


namespace jpc_tracer
{
    BasicMaterial::BasicMaterial(std::shared_ptr<ColorValue> color, Prec illumination=0)
        : _color(color), _illumination(illumination), _lambertian(std::make_shared<LambertianBSDF>())
    {
        
    }


    SpectrumPasses BasicMaterial::Illumination(const SurfaceInteraction& interaction, const Ray& ray) const 
    {
        Spectrum color = Spectrum::FromRGB(_color->GetColor(interaction.UV));
        return SpectrumPasses(color *_illumination);
    }
    
    void BasicMaterial::OverrideBSDF(BsdfMemoryPtr& memory, const SurfaceInteraction& interaction) const 
    {
        ResetBsdfMemory(memory, interaction.Normal);
        LambertianParams* params = _lambertian.Setup(memory,1);
        Spectrum color = Spectrum::FromRGB(_color->GetColor(interaction.UV));
        params->Reflectance = color;
    }
    
    BsdfMemoryInfo BasicMaterial::GetBsdfInfo() const 
    {
        BsdfMemoryInfo info;
        info.max_bsdf_count = 1;
        auto size = _lambertian.GetMaxSize();
        info.max_byte_size = size;
        return info;
    }
    

}