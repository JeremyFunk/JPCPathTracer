#pragma once
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer
{
    
    struct DebugBsdfClosure final: public IBsdfClosure
    {

        DebugBsdfClosure(Spectrum color = Black())
        :m_color(color)
        {
        }
        constexpr ShaderResult Eval(Ray incident_ray) const
        {
            return {m_color,1};
        }
        
        constexpr ShaderResult Sample2D(Ray* out_incident_ray,Vec2 rand_p) const
        {
            *out_incident_ray = Ray{{m_color[0]*rand_p[0],m_color[1],m_color[2]}};
            return {m_color*rand_p[0],1};
        }
    
        Spectrum m_color;
    };

    constexpr BsdfNode DebugBsdf(ShaderContext* ctx, Spectrum color)
    {
        return CreateBsdf<MATERIAL_DIFFUSE |MATERIAL_GLOSSY, DebugBsdfClosure>(ctx,color);
    }





}