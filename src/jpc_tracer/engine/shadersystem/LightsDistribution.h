#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/engine/shadersystem/Lights.h"
#include "lights/PointLights.h"

namespace jpctracer::shadersys
{
    class LightsDistribution 
    {
    public:
        LightsDistribution(const Lights* lights,SurfaceInteraction interaction);
        std::pair<Spectrum,Prec> operator()(const Ray& incident_ray) const;

        std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                            Vec2 random_point) const;

        std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                            Vec3 random_point) const;
    private:
        const PointLights* m_point_lights;
        SurfaceInteraction m_interaction;
    };
}