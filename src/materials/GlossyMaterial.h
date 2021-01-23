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

namespace jpc_tracer
{
    class GlossyMaterial final: public IMaterial
    {
    public:
        GlossyMaterial(std::shared_ptr<ColorValue> color,Prec roughness);
        virtual SpectrumPasses Illumination(const SurfaceInteraction& interaction, const Ray& ray) const override;
        //virtual IBSDF* ComputeBSDF(const SurfaceInteraction& interaction, MemoryArea& memory_area) const override;

        virtual void OverrideBSDF(BsdfMemoryPtr& memory, const SurfaceInteraction& interaction) const override;
        virtual BsdfMemoryInfo GetBsdfInfo() const override;
        ~GlossyMaterial();
        
    private:
        std::shared_ptr<ColorValue> _color;
        std::shared_ptr<BsdfGeneric<CookTorranceParams<DielectricFresnelParams, BeckmannParams>>> _bsdf;
        Prec _alpha;
    };

    Prec RoughnessToAlpha(Prec roughness);
}