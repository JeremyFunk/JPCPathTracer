#pragma once
#include "jpc_tracer/core/core.h"
#include "cache/ShaderCache.h"
#include "cache/SharedMemory.h"
#include "BaseFunctions.h"

namespace jpctracer::shader::detail {

    //Innerbsdf Concepts
    //**************************************************************************

    class IBsdfClosure
    {
    public:
        virtual std::pair<Spectrum,Prec> operator()(const SharedMemory& mem,
                            const Ray& incident_ray) const = 0;

        virtual std::pair<Spectrum,Prec> operator()(const SharedMemory& mem,
                        Ray* out_incident_ray, Vec2 random_point)
        {
            return {Black(),0};
        }

        virtual std::pair<Spectrum,Prec> operator()(const SharedMemory& mem,
                            Ray* out_incident_ray, Vec3 random_point) const
        {
            return {Black(),0};
        }

    };

    template<class T>
    concept BsdfClosure = requires()
    {
        T{};
    }
    &&  std::derived_from<T,IBsdfClosure>;

    template<class T,MaterialType type>
    concept __BsdfCreator = requires(const T& creator, 
                                const SharedMemory mem)
    {
        {creator.template CreateDistribution<type>(mem)}
            -> BsdfClosure;
    };

    template<class T>
    concept BsdfCreator = __BsdfCreator<T, MaterialType::EMISSION> 
                        || __BsdfCreator<T, MaterialType::BSDF>;
    

    template<class T>
    concept BsdfBuilder = requires(T builder, ShaderCache& cache)
    {
        {builder.Build(cache)}->BsdfCreator;
    };

    //Value
    //**************************************************************************

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

namespace jpctracer::shader {
    template<class T>
    concept ColorBuilder = requires(T builder, ShaderCache& cache)
    {
        {detail::Build(builder, cache)}->detail::Color;
    };

    template<class T>
    concept ValueBuilder = requires(T builder, ShaderCache& cache)
    {
        {detail::Build(builder,cache)}->detail::Value;
    };

    template<class T>
    concept NormalBuilder = requires(T builder, ShaderCache& cache)
    {
        {detail::Build(builder,cache)}->detail::Normal;
    };
    namespace detail {

        template<class T1, class T2>
        concept SameBuilder = ( NormalBuilder<T1> && NormalBuilder<T2>) ||
            (ValueBuilder<T1> && ValueBuilder<T2>) ||
            (ColorBuilder<T1> && ColorBuilder<T2>);
        
    }

    static_assert(ColorBuilder<Spectrum>);
    static_assert(ValueBuilder<Prec>);
    static_assert(NormalBuilder<Vec3>);
}