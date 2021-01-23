#pragma once
#include "Linalg.h"
#include "Ray.h"
#include "core/Spectrum.h"
namespace jpc_tracer {
    struct LightInformation{
    public:
        LightInformation(Vec3 direction, Prec distance){
            Direction = direction;
            Distance = distance;
        }
        Vec3 Direction; Prec Distance;
    };

    class ILight
    {
    public:
        virtual LightInformation GetLightInformation(const Vec3& point) const = 0;
        virtual Spectrum Illumination(const Vec3& position, const LightInformation& lightInformation) const = 0;
    };
}