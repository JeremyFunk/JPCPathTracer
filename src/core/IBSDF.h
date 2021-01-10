#pragma once
#include "Spectrum.h"
#include "SurfaceInteraction.h"
#include "Ray.h"
#include "core/SpectrumPasses.h"
namespace core {
    struct BSDFResult
    {
    public:
        BSDFResult(SpectrumPasses scattering,
        float pdf,  Vec3 incident_direction) : Scattering(scattering),Pdf(pdf), IncidentDirection(incident_direction) {}

        BSDFResult(){}

        SpectrumPasses Scattering;
        Vec3 IncidentDirection;
        float Pdf = 1;    
    };
    struct BxDFType
    {
        bool BSDF_REFLECTION = true; 
        bool BSDF_TRANSMISSION = true;
        bool BSDF_DIFFUSE = true;
        bool BSDF_GLOSSY = true;
        bool BSDF_SPECULAR = true;
    };
    class IBSDF
    {
    public:
        //all Direction should be normalized
        virtual BSDFResult EvaluateAll(const Vec3& scattered_direction,const Vec2& random_point,BxDFType type = BxDFType()) const = 0;
        virtual SpectrumPasses Scattering(const Vec3& scattered_direction,const Vec3& incident_direction) const = 0;
        virtual Prec Pdf(const Vec3& scattered_direction,const Vec3& incident_direction) const = 0;
        virtual Vec3 SampleIncidentDirection(const Vec3& scattered_direction, const Vec2& random_point) const = 0;

    };
}