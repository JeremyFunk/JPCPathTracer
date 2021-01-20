#pragma once
#include "core/IMaterial.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "color/ColorValue.h"
#include "core/BsdfGeneric.h"
#include "bsdfs/LambertianBSDF.h"
namespace materials
{

    class BasicMaterial : public core::IMaterial
    {
    public:
        BasicMaterial(std::shared_ptr<color::ColorValue> color, core::Prec illumination);
        virtual core::SpectrumPasses Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const override;
        virtual void OverrideBSDF(core::BsdfMemoryPtr& memory, const core::SurfaceInteraction& interaction) const override;
        virtual core::BsdfMemoryInfo GetBsdfInfo() const override;
    private:
        std::shared_ptr<color::ColorValue> _color;
        core::Prec _illumination;
        core::BsdfGeneric<bsdfs::LambertianParams> _lambertian;
    };
}