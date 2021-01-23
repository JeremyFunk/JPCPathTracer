#include "PointLight.h"
#include "core/constants.h"



namespace jpc_tracer
{
    PointLight::PointLight(const Vec3& position, const Spectrum& spectrum){
        _spectrum = spectrum;
        _position = position;
    }

    LightInformation PointLight::GetLightInformation(const Vec3& point) const 
    {
        Vec3 dir = (point - _position);
        return LightInformation(dir.normalized(), dir.norm());
    }
    
    Spectrum PointLight::Illumination(const Vec3& point, const LightInformation& lightInformation) const 
    {
        return _spectrum / (4 * M_PI * lightInformation.Distance * lightInformation.Distance);
    }
}