#pragma once
#include "jpc_tracer/core/archetypes.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/plugins/shaders/cache/ShaderCache.h"
#include "BsdfSharedMemory.h"

namespace jpctracer::shader {
    


    namespace detail {


        template<class T>
        struct OptimizedToClosureWapper
        {
            OptimizedToClosureWapper(T closure,BsdfSharedMemory memory) 
            : m_closure(closure), m_memory(memory){}

            std::pair<Spectrum,Prec> operator()(const Ray& incident_ray) const
            {
                Ray incident_ray_ns = WorldToNormal(incident_ray,m_memory);
                return m_closure(m_memory,incident_ray_ns);
            }

            std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                                Vec2 random_point) const
            {
                auto result = m_closure(m_memory,out_incident_ray,random_point);
                *out_incident_ray = NormalToWorld(*out_incident_ray,m_memory);
                return result;
            }

            std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                                Vec3 random_point) const
            {
                auto result = m_closure(m_memory,out_incident_ray,random_point);
                *out_incident_ray = NormalToWorld(*out_incident_ray,m_memory);
                return result;
            }

            T m_closure;

            BsdfSharedMemory m_memory;
        };

        

    
        template<class T>
        struct OptimizedToBsdfWapper
        {
            OptimizedToBsdfWapper(T bsdf) : m_bsdf(bsdf){}
            template<MaterialType type>
            auto CreateDistribution(const Ray& scattering_ray,
                                    const SurfaceInteraction& interaction)
            {
                auto mem = shader::CreateSharedMemory(scattering_ray, 
                                                                interaction);
                auto closure = m_bsdf.CreateDistribution(mem);
                return shader::detail::OptimizedToClosureWapper(std::move(closure),
                                                                std::move(mem));
            }

            template<MaterialType type>
            auto CreateDistribution(const BsdfSharedMemory& mem) 
            {
                return m_bsdf.CreateDistribution<type>(mem);
            }

            T m_bsdf;
        };

        template<class T>
        struct ClosureToOptimizedWapper
        {
            ClosureToOptimizedWapper(T closure) 
            : m_closure(closure){}

            std::pair<Spectrum,Prec> operator()(const BsdfSharedMemory& memory ,
                                            const Ray& incident_ray) const
            {
                return m_closure(NormalToWorld(incident_ray,memory));
            }

            std::pair<Spectrum,Prec> operator()(const BsdfSharedMemory& memory ,
                                                Ray* out_incident_ray, 
                                                Vec2 random_point) const
            {
                auto result = m_closure(out_incident_ray,random_point);
                *out_incident_ray = NormalToWorld(*out_incident_ray,memory);
                return result;
            }

            std::pair<Spectrum,Prec> operator()(const BsdfSharedMemory& memory ,
                                                Ray* out_incident_ray, 
                                                Vec3 random_point) const
            {
                auto result = m_closure(out_incident_ray,random_point);
                *out_incident_ray = NormalToWorld(*out_incident_ray,memory);
                return result;
            }

            T m_closure;

        };

        template<class T>
        struct BsdfToOptimizedWapper
        {
            BsdfToOptimizedWapper(T bsdf) : m_bsdf(bsdf){}
            template<MaterialType type>
            auto CreateDistribution(const Ray& scattering_ray,
                                    const SurfaceInteraction& interaction)
            {
               return m_bsdf.template CreateDistribution<type>(
                                                    scattering_ray,interaction);
            }

            template<MaterialType type>
            auto CreateDistribution(const BsdfSharedMemory& mem) 
            {
                auto closure =  m_bsdf.template CreateDistribution<type>(
                                        mem.ScatteringRay, mem.Interaction);
                return ClosureToOptimizedWapper(std::move(closure));
            }

            T m_bsdf;
        };


    }

    template<class T>
    concept OptimizedBuilder = requires(T t,plugins::ShaderCache& cache) 
            {t.BuildOptimized(cache);};

    template<class T>
    concept NormalBuilder = requires(T t,plugins::ShaderCache& cache) 
            {t.Build(cache);}
            && (! OptimizedBuilder<T>);

    template<OptimizedBuilder T>
    auto Build(T builder,plugins::ShaderCache& cache)
    {
        auto bsdf = builder.BuildOptimized(cache);
        return detail::OptimizedToBsdfWapper(bsdf);
    }

    template<NormalBuilder T>
    auto Build(T builder,plugins::ShaderCache& cache)
    {
        return builder.Build(cache);
    }
    

    template<OptimizedBuilder T>
    auto BuildOptimized(T builder,plugins::ShaderCache& cache)
    {
        return builder.BuildOptimized(cache);
    }

    template<NormalBuilder T>
    auto BuildOptimized(T builder,plugins::ShaderCache& cache)
    {
        auto bsdf = builder.Build(cache);
        return detail::BsdfToOptimizedWapper(bsdf);
    }

}