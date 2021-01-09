#include "DirectionalLight.h"
#include "core/constants.h"
#include <limits.h>


namespace lights
{
    DirectionalLight::DirectionalLight(const Vec3& direction, const Spectrum& spectrum){
        _spectrum = spectrum;
        _direction = direction;
    }

    LightInformation DirectionalLight::GetLightInformation(const Vec3& point) const 
    {   
        return LightInformation(_direction, std::numeric_limits<Prec>::infinity());
    }
    
    Spectrum DirectionalLight::Illumination(const Vec3& point, const LightInformation& lightInformation) const 
    {
        return _spectrum;
    }
}