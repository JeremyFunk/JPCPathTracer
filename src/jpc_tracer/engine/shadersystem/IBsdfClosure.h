#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "NormalSpace.h"

namespace jpctracer::shadersys {

    struct ShaderResult
    {
        Spectrum luminance = FromValue(0);
        Prec pdf = 0;
    };

    inline ShaderResult operator+(const ShaderResult& a,const ShaderResult& b)
    {
        return {a.luminance+b.luminance,a.pdf+b.pdf};
    }

    inline void operator+=(ShaderResult& a,const ShaderResult& b)
    {
        a.luminance+=b.luminance;
        a.pdf+=b.pdf;
    }

    inline ShaderResult operator*(const ShaderResult& a,const Prec& v)
    {
        return {a.luminance*v,a.pdf*v};
    }


    inline void operator*=(ShaderResult& a,const Prec& v)
    {
        a.luminance*=v;
        a.pdf*=v;
    }

    

    struct IBsdfClosure
    {
        virtual ShaderResult Eval( Ray incident_ray) const = 0;
        virtual ShaderResult Sample2D( Ray* out_incident_ray,Vec2 rand_p) const
        {
            return {Black(),0};
        }
        virtual ShaderResult Sample3D( Ray* out_incident_ray,Vec3 rand_p) const
        {
            return {Black(),0};
        }
    };
}