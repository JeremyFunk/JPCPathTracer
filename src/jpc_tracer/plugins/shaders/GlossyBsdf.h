#pragma once
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/shadersystem/IBsdfClosure.h"
#include "Fresnel.h"

namespace jpctracer
{
    //Reference Microfacet Models for Refraction through Rough Surfaces


    inline Prec Sign(const Prec& x)
    {
        return x < 0 ? -1 : 1; 
    }
    inline Prec GreaterZero(const Prec& x)
    {
        return x > 0 ? 1 : 0; 
    }
    inline Vec3 MicrofacetNormal(Vec3 incident, Vec3 scattered)
    {
        Vec3 result = Sign(incident[2]) * (incident + scattered);
        return result.normalized();
    };


    
    inline Prec BeckmanDistribution(Vec3 microfacet_normal,Prec alpha2)
    {
        Prec cos2 = Cos2Theta(microfacet_normal);
        Prec tan2 = Tan2Theta(cos2);

        return GreaterZero(microfacet_normal[2]) 
        * std::exp(-tan2 / alpha2) / (M_PI * alpha2* cos2*cos2);

    };
    //Microfacet Models for Refraction through Rough Surfaces
    inline Prec BeckmanShadowSingle(Vec3 v, Vec3 microfacet_normal,Prec alpha)
    {
        return GreaterZero(v.dot(microfacet_normal) / v[2]) *
            alpha < 1.6 ?
                3.535 * alpha + 2.181 * alpha*alpha /
                (1 + 2.276 * alpha + 2.577 * alpha * alpha)
            :
                1;
    }
    //Pbrt
    inline Prec BeckmannLambda(Prec alpha,const Vec3& direction) 
    {
        Prec tan_theta = TanTheta(direction);
        if(std::isinf(tan_theta))
            return 0;

        Prec a = 1 / (tan_theta*alpha);

        return a<1.6 ?
            (1-1.259f * a + 0.396f * a * a) / (3.525f * a + 2.181f *a*a)
        :
            0;
    }

    inline Prec BeckmanShadow(Prec alpha,const Vec3& scattered_direction,const Vec3& incident_direction)
    {
        return 1 / (1+BeckmannLambda(alpha, scattered_direction) + BeckmannLambda(alpha, incident_direction));
    }

    inline Vec3 BeckmannSample(Prec alpha2,Vec2 random_point)
    {
        Prec log_sample = std::log(random_point[0]);
        if(std::isinf(log_sample))
            log_sample = 0;

        Prec tan2_theta = -alpha2 * log_sample;
        Prec phi = random_point[1] * 2 *M_PI;
        Prec cos_theta = 1 / std::sqrt(1+tan2_theta);
        Prec sin_theta = std::sqrt(std::max(0.f,1-cos_theta*cos_theta));
        Prec x = sin_theta*std::cos(phi);
        Prec y = sin_theta*std::sin(phi);
        Prec z = cos_theta;

        return {x,y,z};
    }

    inline ShaderResult CookTorranceReflection(Vec3 incident_dir ,Vec3 scattered_dir, Spectrum reflectance,
        Prec alpha)
    {
        ShaderResult result;

        Prec alpha2 = alpha*alpha;

        Vec3 microfacet_normal = MicrofacetNormal(incident_dir, scattered_dir);
        Prec G = BeckmanShadow(alpha,scattered_dir,incident_dir);
        Prec D = BeckmanDistribution(microfacet_normal, alpha2);
        Prec F = frenel_noop();

        result.pdf = D * std::abs(microfacet_normal[2]) / (4*microfacet_normal.dot(scattered_dir));
        result.luminance = reflectance * F * G * D /
                         (4 * incident_dir[2]*scattered_dir[2] );
    
        return result;
    }

    inline Vec3 CookTorranceSample(Vec3 scattered_dir,Prec alpha2,Vec2 random_point)
    {
        /*
        Math:
            s: scattered_dir
            i: incident_dir
            m: microfacet_normal
        (s+i) * a = m

        */
        Vec3 microfacet_normal = BeckmannSample(alpha2, random_point);
        Vec3 incident_direction = Reflect(scattered_dir, microfacet_normal);

        if(! SameHemiSphereNormalSpace(incident_direction, scattered_dir))
            return -1 * incident_direction;
        return incident_direction;
    }

    inline Prec RoughnessToAlpha(Prec roughness) {
        roughness = std::max(roughness, (Prec)1e-3);
        Prec x = std::log(roughness);
        return 1.62142f + 0.819955f * x + 0.1734f * x * x +
               0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
    }
    
    struct GlossyBsdfClosure final: public IBsdfClosure
    {

        GlossyBsdfClosure(Spectrum color,Prec roughness, const Ray& scattering_ray)
        :m_color(color), m_alpha(RoughnessToAlpha(roughness)), m_scattering_dir(scattering_ray.Direction),
        m_time(scattering_ray.Time)
        {
        }
        inline ShaderResult Eval(Ray incident_ray) const
        {
            return CookTorranceReflection(incident_ray.Direction,m_scattering_dir,m_color,m_alpha);
        }
        
        inline ShaderResult Sample2D(Ray* out_incident_ray,Vec2 rand_p) const
        {
            *out_incident_ray = Ray{
                            CookTorranceSample(m_scattering_dir,m_alpha*m_alpha, rand_p),
                            {0,0,0},
                            {0,0},
                            10000,
                            m_time
                            };
            return CookTorranceReflection(out_incident_ray->Direction, m_scattering_dir, m_color, m_alpha);

        }
    
        Spectrum m_color;
        Prec m_alpha;
        const Vec3& m_scattering_dir;
        const Prec m_time;
    };
    
    constexpr BsdfNode GlossyBsdf(ShaderContext& ctx,Spectrum color,Prec roughness)
    {
        return CreateBsdf<MATERIAL_GLOSSY, GlossyBsdfClosure>(ctx,color,roughness, ctx.normal_space.ScatteringRay);
    }

}