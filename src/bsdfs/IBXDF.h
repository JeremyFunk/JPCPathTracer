#pragma once
#include "core/IBSDF.h"
#include "core/Spectrum.h"
namespace bsdfs {
    struct BXDFDirectionInfo
    {
        BXDFDirectionInfo(core::Spectrum surface_color,
        float pdf,  core::Vec3 outgoing_direction) : SurfaceColor(surface_color),Pdf(pdf), OutgoingDirection(outgoing_direction) {}

        BXDFDirectionInfo(){}

        core::Spectrum SurfaceColor;
        core::Vec3 OutgoingDirection;
        float Pdf = 1;  
    };
    class IBXDF
    {
    public:
        //ns for normal space
        virtual BXDFDirectionInfo Evaluate(const core::Vec3& incoming_direction_ns,const core::Vec2& random_point,
            core::BxDFType type = core::BxDFType()) = 0;
        core::BxDFType Type;

    };
}