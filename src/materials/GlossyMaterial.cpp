#include "GlossyMaterial.h"
#include "bsdfs/BeckmannDistribution.h"
#include "bsdfs/DielectricFresnel.h"
#include "core/Bsdf.h"
#include "core/BsdfGeneric.h"
#include "core/Spectrum.h"
#include <memory>

namespace materials
{
    GlossyMaterial::GlossyMaterial(std::shared_ptr<color::ColorValue> color,core::Prec roughness) 
        : _alpha(RoughnessToAlpha(roughness)), _color(color)
    {
        auto fresnel = std::make_shared<bsdfs::DielectricFresnel>();
        auto distribution = std::make_shared<bsdfs::BeckmannDistribution>();

        using BsdfParams = bsdfs::CookTorranceParams<bsdfs::DielectricFresnelParams, bsdfs::BeckmannParams>;
        using BsdfClosure = bsdfs::CookTorranceBSDFClosure<bsdfs::DielectricFresnelParams, bsdfs::BeckmannParams>;


        std::shared_ptr<core::BsdfClosureGeneric<BsdfParams>> bsdf_closure = std::make_shared<BsdfClosure>(fresnel,distribution);
        _bsdf = std::make_shared<core::BsdfGeneric<BsdfParams>>(bsdf_closure);
    }
    
    core::SpectrumPasses GlossyMaterial::Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const 
    {
        return core::Spectrum::FromValue(0);
    }

    core::Prec RoughnessToAlpha(core::Prec roughness) {
        roughness = std::max(roughness, (core::Prec)1e-3);
        core::Prec x = std::log(roughness);
        return 1.62142f + 0.819955f * x + 0.1734f * x * x +
               0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
    }


    void GlossyMaterial::OverrideBSDF(core::BsdfMemoryPtr& memory, const core::SurfaceInteraction& interaction) const
    {
        core::ResetBsdfMemory(memory, interaction.Normal);
        auto params = _bsdf->Setup(memory, 1);
        params->Reflectance=core::Spectrum::FromRGB(_color->GetColor(interaction.UV));
        params->Fresnel.IndexOfRefractionInner = 1.45;
        params->Fresnel.IndexOfRefractionOuter = 1;
        params->Distribution.Alpha = _alpha;

    }
    
    core::BsdfMemoryInfo GlossyMaterial::GetBsdfInfo() const 
    {
        core::BsdfMemoryInfo info = {0,0};
        info.max_byte_size = _bsdf->GetMaxSize();
        info.max_bsdf_count = 1;
        return info;
    }
    
    GlossyMaterial::~GlossyMaterial() 
    {
        
    }
}