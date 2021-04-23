#pragma once
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer
{

struct DebugBsdfClosure final : public IBsdfClosure
{

    DebugBsdfClosure(Spectrum color = Black()) : m_color(color)
    {
    }
    inline ShaderResult Eval(Ray incident_ray) const
    {
        return {m_color, 1};
    }

    inline Norm3 Sample2D(Vec2 rand_p) const
    {
        return Vec3{m_color[0] * rand_p[0], m_color[1], m_color[2]};
    }

    Spectrum m_color;
};

inline BsdfNode DebugBsdf(Spectrum color)
{
    return CreateBsdf(MATERIAL_DIFFUSE, DebugBsdfClosure(color));
}

struct DebugMaterial
{
    Spectrum color = FromRGB({1, 1, 1});
    auto bsdf()
    {
        return [=](Ray scattered) { return DebugBsdf(color); };
    }
};

} // namespace jpctracer