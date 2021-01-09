#pragma once
#include "core/IMaterial.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "color/ColorValue.h"
namespace materials
{

    class BasicMaterial : public core::IMaterial
    {
    public:
        BasicMaterial(std::shared_ptr<color::ColorValue> color, core::Prec illumination);
        virtual core::SpectrumPasses Illumination(const core::SurfaceInteraction& interaction, const core::Ray& ray) const override;
        virtual core::IBSDF* ComputeBSDF(const core::SurfaceInteraction& interaction, core::MemoryArea& memory_area) const override;  
    private:
        std::shared_ptr<color::ColorValue> _color;
        core::Prec _illumination;
    };
}