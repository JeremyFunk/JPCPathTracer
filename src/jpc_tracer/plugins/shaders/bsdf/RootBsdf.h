#pragma once
#include "../Concepts.h"
#include "jpc_tracer/core/core.h"
#include "MixBsdf.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/plugins/shaders/cache/ShaderCache.h"
#include <tuple>
#include <utility>
#include "../sampling/DiscreteSampler.h"

namespace jpctracer::shader::detail {

    
    

    template<class... DistsT>
    struct RootBsdfClosure
    {
        RootBsdfClosure(std::tuple<DistsT...> closures,
                        std::array<Prec,sizeof...(DistsT)> weights,
                        SharedMemory mem)
            : m_closures(std::move(closures)), m_weights(weights),
            m_memory(std::move(mem)), m_weights_sampler(weights)
        {
            
            std::apply([&](DistsT&... args)
            {
                int i = 0;
                auto f = [&](const IBsdfClosure* closure)
                {
                    m_virtual_closures[i] = closure;
                    i++;
                };
                //foreach for args
                (f(&args),...);
            },m_closures);

        };
        std::pair<Spectrum,Prec> operator()(const Ray& incident_ray) const
        {
            Prec pdf = 0;
            Spectrum spec = FromValue(0.f);
            int i = 0;
            for(const auto& closure : m_virtual_closures)
            {
                
                auto[closure_spec,closure_pdf] = closure->operator()(m_memory,incident_ray);
                Prec weight = m_weights[i];
                spec+=closure_spec * weight;
                pdf+=closure_pdf * weight;

                i++;
            }
            return {spec,pdf};
        }

        std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                            const Vec2& random_point) const
        {
            return compute_sampled_bsdf(out_incident_ray,random_point);
        }

        std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                            const Vec3& random_point) const
        {
            return compute_sampled_bsdf(out_incident_ray,random_point);
        }

    private:


        std::tuple<DistsT...> m_closures;
        std::array<Prec,sizeof...(DistsT)> m_weights;
        DiscreteSampler<sizeof...(DistsT)> m_weights_sampler;
        
        std::array<const IBsdfClosure*,sizeof...(DistsT)> m_virtual_closures;
        SharedMemory m_memory;

        template<class T>
        std::pair<Spectrum,Prec> compute_sampled_bsdf(Ray* out_incident_ray,
                                T rand_p) const
        {
            auto[sampled_idx,rand_x] = m_weights_sampler.Sample(rand_p[0]);
            rand_p[0] = rand_x;
            const IBsdfClosure* sampled_closure = m_virtual_closures[sampled_idx];
            auto[spec,pdf] = *(sampled_closure)(out_incident_ray,rand_p);

            Prec weight = m_weights[sampled_idx];
            spec *=weight;
            pdf *=weight;

            int i = 0;
            for(const auto& closure : m_virtual_closures)
            {
                if(i==sampled_idx) continue;

                auto[closure_spec,closure_pdf] = closure->operator()(m_memory,out_incident_ray);
                weight = m_weights[i];
                spec+=closure_spec * weight;
                pdf+=closure_pdf * weight;

                i++;
            }
            
        }
    };

    //_Size=0
    template<>
    struct RootBsdfClosure<>
    {

        RootBsdfClosure(std::tuple<> closures,
                        std::array<Prec,0> weights,
                        SharedMemory mem) {}

        std::pair<Spectrum,Prec> operator()(const Ray& incident_ray) const
        {
            return {Black(),0};
        }
        std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                            const Vec2& random_point) const
        {
            return {Black(),0};
        }

        std::pair<Spectrum,Prec> operator()(Ray* out_incident_ray, 
                                            const Vec3& random_point) const
        {
            return {Black(),0};
        }
    };

    template<MaterialType type, class CreatorT >
    auto CreateDistribution(const CreatorT& creator, const Ray& scattering_ray,
                    const SurfaceInteraction& interaction)
    {
        SharedMemory memory = CreateSharedMemory(scattering_ray, 
                                                    interaction);
        using closures_t = typename FlattenNodes<type, CreatorT>::type;
        constexpr size_t length = std::tuple_size<closures_t>::value;

        

        closures_t closures{};

        std::array<Prec,length> weights;
        weights.fill(1.f);

        SetupMixer<type,0>(creator,closures,weights,memory);

        return RootBsdfClosure(std::move(closures),std::move(weights),memory);        
    }





}