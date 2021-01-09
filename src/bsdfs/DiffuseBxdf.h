#pragma once
#include "IBXDF.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"

namespace bsdfs
{
    class DiffuseBxdf final : public bsdfs::IBXDF
    {
    public:
        DiffuseBxdf(core::Spectrum surface_color);
        virtual BXDFDirectionInfo Evaluate(const core::Vec3& incoming_direction_ns,const core::Vec2& random_point,core::BxDFType type = core::BxDFType()) override;
        core::BxDFType Type;

    private:
        core::Spectrum _surface_color;
    };
}