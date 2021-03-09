#pragma once
#include "jpc_tracer/maths/maths.h"
#include "jpc_tracer/shaders/Cache.h"
namespace jpctracer {
    
    //Forward Declaration

    namespace shader {
        
        template<class T>
        auto Eval(T c, const SurfaceInteraction& interaction)
        {
            return c.Eval(interaction);
        }

        constexpr Spectrum Eval(Spectrum c, const SurfaceInteraction& interaction) {return c;}
        constexpr Prec Eval(Prec v, const SurfaceInteraction& interaction) {return v;}
        constexpr Vec3 Eval(Vec3 n, const SurfaceInteraction& interaction) {return n;}
        
        template<class T>
        auto Build(T c,Cache& cache)
        {
            return c.Build(cache);
        }
        
        constexpr Spectrum Build(Spectrum c, Cache& cache) {return c;}
        constexpr Prec Build(Prec v, Cache& cache) {return v;}
        constexpr Vec3 Build(Vec3 n, Cache& cache) {return n;}

        template<class T>
        concept Color = requires(T color, SurfaceInteraction& interaction)
        {
            {Eval(color,interaction)}->std::convertible_to<Spectrum>;
        };

        template<class T>
        concept Value = requires(T value)
        {
            {Eval(value,SurfaceInteraction())}->std::convertible_to<Prec>;
        };

        template<class T>
        concept Normal = requires(T normal)
        {
            {Eval(normal,SurfaceInteraction())}->std::convertible_to<Vec3>;
        };


        
    }    

    template<class T>
    concept ColorBuilder = requires(T builder, shader::Cache& cache)
    {
        {shader::Build(builder, cache)}->shader::Color;
    };

    template<class T>
    concept ValueBuilder = requires(T builder, shader::Cache& cache)
    {
        {shader::Build(builder,cache)}->shader::Value;
    };

    template<class T>
    concept NormalBuilder = requires(T builder, shader::Cache& cache)
    {
        {shader::Build(builder,cache)}->shader::Normal;
    };
    namespace shader {

        template<class T1, class T2>
        concept SameBuilder = ( NormalBuilder<T1> && NormalBuilder<T2>) ||
            (ValueBuilder<T1> && ValueBuilder<T2>) ||
            (ColorBuilder<T1> && ColorBuilder<T2>);
        
    }

    static_assert(ColorBuilder<Spectrum>);
    static_assert(ValueBuilder<Prec>);
    static_assert(NormalBuilder<Vec3>);
    

}