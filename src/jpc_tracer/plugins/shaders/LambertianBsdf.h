#pragma once
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer {
    struct LambertianBsdfClosure final: public IBsdfClosure
    {

        LambertianBsdfClosure(Spectrum color, const Ray& scattered_ray)
            : m_color(color), m_scatered_dir(scattered_ray.Direction),m_time(scattered_ray.Time)
        {

        }
        inline ShaderResult Eval(Ray incident_ray) const
        {
            Spectrum luminance = m_color / M_PI;
            Prec pdf = 
            SameHemiSphereNormalSpace(m_scatered_dir, incident_ray.Direction) ?
                pdf = std::abs(CosTheta(incident_ray.Direction)) / M_PI 
            :
                0;
            return {luminance,pdf};
        }
        
        inline ShaderResult Sample2D(Ray* out_incident_ray,Vec2 rand_p) const
        {
            auto incident_dir = CosinusSampleHemisphere(rand_p);

            if( ! SameHemiSphereNormalSpace(incident_dir, m_scatered_dir)) incident_dir*=-1; //flip

            Prec pdf =std::abs(CosTheta(incident_dir)) / M_PI;
            Spectrum luminance = m_color / M_PI;

            *out_incident_ray = Ray{
                            incident_dir,
                            {0,0,0},
                            {0,0},
                            10000,
                            m_time
                            };
            return {luminance,pdf};
        }

        Spectrum m_color;
        Vec3 m_scatered_dir;
        Prec m_time;
    
 
    };

    constexpr BsdfNode DiffuseBsdf(ShaderContext& ctx,Spectrum color)
    {
        return CreateBsdf<MATERIAL_DIFFUSE, LambertianBsdfClosure>(ctx,color, ctx.normal_space.ScatteringRay);
    }
}