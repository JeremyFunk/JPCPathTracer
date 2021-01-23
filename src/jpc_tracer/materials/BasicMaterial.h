#pragma once
#include "core/IMaterial.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "color/ColorValue.h"
#include "core/BsdfGeneric.h"
#include "bsdfs/LambertianBSDF.h"
namespace jpc_tracer
{

    class BasicMaterial : public IMaterial
    {
    public:
        BasicMaterial(Ref<ColorValue> color, Prec illumination);
        virtual SpectrumPasses Illumination(const SurfaceInteraction& interaction, const Ray& ray) const override;
        virtual void OverrideBSDF(BsdfMemoryPtr& memory, const SurfaceInteraction& interaction) const override;
        virtual BsdfMemoryInfo GetBsdfInfo() const override;
    private:
        Ref<ColorValue> _color;
        Prec _illumination;
        BsdfGeneric<LambertianParams> _lambertian;
    };
}