#pragma once

#include "bsdfs/BeckmannDistribution.h"
#include "bsdfs/CookTorranceBSDF.h"
#include "bsdfs/DielectricFresnel.h"
#include "bsdfs/LambertianBSDF.h"
#include "color/ColorValue.h"
#include "core/BsdfGeneric.h"
#include "core/IMaterial.h"
#include "GlossyMaterial.h"
namespace jpc_tracer
{
    class GlossyDiffMaterial : public IMaterial
    {
    public:
        GlossyDiffMaterial(Ref<ColorValue> diffuse_color, Ref<ColorValue> glossy_color,Prec roughness, Prec mix_factor);
        virtual SpectrumPasses Illumination(const SurfaceInteraction& interaction, const Ray& ray) const;
        virtual void OverrideBSDF(BsdfMemoryPtr& memory, const SurfaceInteraction& interaction) const;
        virtual BsdfMemoryInfo GetBsdfInfo() const;
    private:

        Ref<ColorValue> _glossy_color;
        Ref<ColorValue> _diffuse_color;
        Ref<BsdfGeneric<CookTorranceParams<DielectricFresnelParams, BeckmannParams>>> _glossy;
        BsdfGeneric<LambertianParams> _lambertian;
        Prec _alpha;
        Prec _mix_factor;
    };
}