#pragma once
#include "jpc_tracer/core/core.h"

namespace jpctracer::shader::detail {
    template<class T>
    auto Eval(T c, const SurfaceInteraction& interaction)
    {
        return c.Eval(interaction);
    }

    constexpr Spectrum Eval(Spectrum c, const SurfaceInteraction& interaction) 
    {
        return c;
    }
    constexpr Prec Eval(Prec v, const SurfaceInteraction& interaction) 
    {
        return v;
    }
    constexpr Prec Eval(double v, const SurfaceInteraction& interaction) 
    {
        return v;
    }
    constexpr Vec3 Eval(Vec3 n, const SurfaceInteraction& interaction) 
    {
        return n;
    }

    template<class T>
    auto Build(T c,ShaderCache& cache)
    {
        return c.Build(cache);
    }
    
    constexpr Spectrum Build(Spectrum c, ShaderCache& cache) {return c;}
    constexpr Prec Build(Prec v, ShaderCache& cache) {return v;}
    constexpr Prec Build(double v, ShaderCache& cache) {return v;}
    constexpr Vec3 Build(Vec3 n, ShaderCache& cache) {return n;}
}