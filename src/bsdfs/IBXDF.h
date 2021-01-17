#pragma once
#include "core/IBSDF.h"
#include "core/Spectrum.h"
namespace bsdfs {
    struct BXDFResult
    {
    public:
        BXDFResult(core::Spectrum scattering,
        float pdf,  core::Vec3 incident_direction) : Scattering(scattering),Pdf(pdf), IncidentDirection(incident_direction) {}

        BXDFResult(){}

        core::Spectrum Scattering;
        core::Vec3 IncidentDirection;
        float Pdf = 1;    
    };
    class IBXDF
    {
    public:
        //all direction are in Normal Space
        //all direction should be on the same hemisphere
        virtual BXDFResult EvaluateAll(const core::Vec3& scattered_direction,const core::Vec2& random_point) const = 0;
        virtual core::SpectrumPasses Scattering(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const = 0;
        virtual core::Prec Pdf(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const = 0;
        virtual core::Vec3 SampleIncidentDirection(const core::Vec3& scattered_direction, const core::Vec2& random_point) const = 0;
        virtual core::BxDFType GetType() const = 0;
        virtual ~IBXDF() {}

    };
}