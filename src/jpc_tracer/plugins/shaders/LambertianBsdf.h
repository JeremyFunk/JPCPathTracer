#pragma once
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer
{
struct LambertianBsdfClosure final : public IBsdfClosure
{

    LambertianBsdfClosure(Spectrum color, const Ray& scattered_ray)
        : m_color(color), m_scatered_dir(scattered_ray.Direction), m_time(scattered_ray.Time)
    {
    }
    inline Distributed<Spectrum> Eval(Ray incident_ray) const
    {
        Spectrum luminance = m_color / M_PI;
        Prec pdf = SameHemiSphereNormalSpace(m_scatered_dir, incident_ray.Direction)
                       ? pdf = std::abs(CosTheta(incident_ray.Direction)) / M_PI
                       : 0;
        return {luminance, pdf};
    }

    inline Norm3 Sample2D(Vec2 rand_p) const
    {
        auto incident_dir = CosinusSampleHemisphere(rand_p);

        if (!SameHemiSphereNormalSpace(incident_dir, m_scatered_dir))
            incident_dir *= -1; // flip

        return incident_dir;
    }

    Spectrum m_color;
    Vec3 m_scatered_dir;
    Prec m_time;
};

inline BsdfNode DiffuseBsdf(Ray scattered, Spectrum color)
{
    return CreateBsdf(MATERIAL_DIFFUSE, LambertianBsdfClosure(color, scattered));
}
} // namespace jpctracer