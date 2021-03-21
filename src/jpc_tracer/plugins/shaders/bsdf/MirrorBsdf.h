#pragma once

#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "OptimizedBsdf.h"
namespace jpctracer
{
    namespace shader {
        
        namespace detail {
        
        
            struct MirrorClosure
            {
                std::pair<Spectrum,Prec> operator()(const BsdfSharedMemory& mem,
                                                const Ray& incident_ray) const
                {
                    return {Spectrum(0),0};
                };

                std::pair<Spectrum,Prec> operator()(const BsdfSharedMemory& mem,
                                                    Ray* out_incident_ray, 
                                                    Vec2 random_point) const
                {return {};}

                std::pair<Spectrum,Prec> operator()(const BsdfSharedMemory& mem,
                                                    Ray* out_incident_ray, 
                                                    Vec3 random_point) const
                {return {};}
            };

            struct MirrorBsdf
            {
                auto CreateDistribution(const BsdfSharedMemory& mem)
                {
                    return MirrorClosure();
                }
            };
            
        }

    
    

        
        struct MirrorBsdfBuilder
        {
            auto BuildOptimized(plugins::ShaderCache& cache)
            {
                return detail::MirrorBsdf();
            }
        };

    }

    

    static_assert(cts::ShaderBuilder<shader::MirrorBsdfBuilder>);
}