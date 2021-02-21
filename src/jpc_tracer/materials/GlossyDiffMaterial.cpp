#include "GlossyDiffMaterial.h"
#include "core/Logger.h"
#include "core/Spectrum.h"
#include "materials/GlossyMaterial.h"



namespace jpc_tracer
{
    GlossyDiffMaterial::GlossyDiffMaterial(Ref<ColorValue> diffuse_color, Ref<ColorValue> glossy_color,Prec roughness, Prec mix_factor) 
        : _diffuse_color(diffuse_color), 
        _glossy_color(glossy_color), 
        _alpha(RoughnessToAlpha(roughness)),
        _lambertian(MakeRef<LambertianBSDF>()),
        _mix_factor(mix_factor)

    {
        auto fresnel = MakeRef<DielectricFresnel>();
        auto distribution = MakeRef<BeckmannDistribution>();

        using BsdfParams = CookTorranceParams<DielectricFresnelParams, BeckmannParams>;
        using BsdfClosure = CookTorranceBSDFClosure<DielectricFresnelParams, BeckmannParams>;


        Ref<BsdfClosureGeneric<BsdfParams>> bsdf_closure = MakeRef<BsdfClosure>(fresnel,distribution);
        _glossy = MakeRef<BsdfGeneric<BsdfParams>>(bsdf_closure);
    }

    SpectrumPasses GlossyDiffMaterial::Illumination(const SurfaceInteraction& interaction, const Ray& ray) const 
    {
        return Spectrum::FromValue(0);
    }
    
    void GlossyDiffMaterial::OverrideBSDF(BsdfMemoryPtr& memory, const SurfaceInteraction& interaction) const
    {
        ResetBsdfMemory(memory, interaction.Normal);
        auto params = _glossy->Setup(memory, 1 - _mix_factor);
        params->Reflectance=Spectrum::FromRGB(_glossy_color->GetColor(interaction.UV));
        params->Fresnel.IndexOfRefractionInner = 1.45;
        params->Fresnel.IndexOfRefractionOuter = 1;
        params->Distribution.Alpha = _alpha;

        auto params2 = _lambertian.Setup(memory, _mix_factor);
        params2->Reflectance = Spectrum::FromRGB(_diffuse_color->GetColor(interaction.UV));
    }
    
    BsdfMemoryInfo GlossyDiffMaterial::GetBsdfInfo() const
    {
        BsdfMemoryInfo info = {0,0};
        info.max_byte_size = _glossy->GetMaxSize();
        info.max_bsdf_count = 1;
        info.max_byte_size+= _lambertian.GetMaxSize();
        info.max_bsdf_count++;
        return info;
    }
}