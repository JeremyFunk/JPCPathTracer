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
        BasicMaterial(std::shared_ptr<ColorValue> color, Prec illumination);
        virtual SpectrumPasses Illumination(const SurfaceInteraction& interaction, const Ray& ray) const override;
        virtual void OverrideBSDF(BsdfMemoryPtr& memory, const SurfaceInteraction& interaction) const override;
        virtual BsdfMemoryInfo GetBsdfInfo() const override;
    private:
        std::shared_ptr<ColorValue> _color;
        Prec _illumination;
        BsdfGeneric<LambertianParams> _lambertian;
    };
}