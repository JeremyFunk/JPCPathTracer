#include "GlossyMaterial.h"
#include "bsdfs/BeckmannDistribution.h"
#include "bsdfs/DielectricFresnel.h"
#include "core/Bsdf.h"
#include "core/BsdfGeneric.h"
#include "core/Spectrum.h"
#include <memory>

namespace jpc_tracer
{
    GlossyMaterial::GlossyMaterial(Ref<ColorValue> color,Prec roughness) 
        : _alpha(RoughnessToAlpha(roughness)), _color(color)
    {
        auto fresnel = MakeRef<DielectricFresnel>();
        auto distribution = MakeRef<BeckmannDistribution>();

        using BsdfParams = CookTorranceParams<DielectricFresnelParams, BeckmannParams>;
        using BsdfClosure = CookTorranceBSDFClosure<DielectricFresnelParams, BeckmannParams>;


        Ref<BsdfClosureGeneric<BsdfParams>> bsdf_closure = MakeRef<BsdfClosure>(fresnel,distribution);
        _bsdf = MakeRef<BsdfGeneric<BsdfParams>>(bsdf_closure);
    }
    
    SpectrumPasses GlossyMaterial::Illumination(const SurfaceInteraction& interaction, const Ray& ray) const 
    {
        return Spectrum::FromValue(0);
    }

    Prec RoughnessToAlpha(Prec roughness) {
        roughness = std::max(roughness, (Prec)1e-3);
        Prec x = std::log(roughness);
        return 1.62142f + 0.819955f * x + 0.1734f * x * x +
               0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
    }


    void GlossyMaterial::OverrideBSDF(BsdfMemoryPtr& memory, const SurfaceInteraction& interaction) const
    {
        //JPC_LOG_INFO("Normal: {},{},{}",interaction.Normal[0],interaction.Normal[1],interaction.Normal[2]);
        
        ResetBsdfMemory(memory, interaction.Normal);
        auto params = _bsdf->Setup(memory, 1);
        params->Reflectance=Spectrum::FromRGB(_color->GetColor(interaction.UV));
        params->Fresnel.IndexOfRefractionInner = 1.45;
        params->Fresnel.IndexOfRefractionOuter = 1;
        params->Distribution.Alpha = _alpha;

    }
    
    BsdfMemoryInfo GlossyMaterial::GetBsdfInfo() const 
    {
        BsdfMemoryInfo info = {0,0};
        info.max_byte_size = _bsdf->GetMaxSize();
        info.max_bsdf_count = 1;
        return info;
    }
    
    GlossyMaterial::~GlossyMaterial() 
    {
        
    }
}