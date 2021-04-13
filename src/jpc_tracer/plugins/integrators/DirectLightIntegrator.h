#pragma once
#include "jpc_tracer/core/Concepts.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "Payload.h"
#include <vector>
#include "ShadowBehavior.h"

namespace jpctracer
{
    struct DirectLightIntegratorSettings
    {
        uint LightSamples;
        uint SubPixels;
    };


    class DirectLightIntegrator
    {
    public:
        void operator()(UInt2 pixel,
            const cts::CameraFunction auto& camera,
            cts::SamplerFunction<Vec2*,Vec3*> auto& sampler,
            cts::TraceRay auto& tracer, 
            cts::Film auto& film);
        
    private:
        uint m_sub_pixels_x;
        uint m_sub_pixels_y;
        uint m_light_samples_x;
        uint m_light_samples_y;
        
        

    };
    void DirectLightIntegrator::operator()(UInt2 pixel,
            const cts::CameraFunction auto& camera,
            cts::SamplerFunction<Vec2*,Vec3*> auto& sampler,
            cts::TraceRay auto& tracer, 
            cts::Film auto& film)
    {
        
        std::vector<Vec2> subpixel_samples(m_sub_pixels_x*m_sub_pixels_y);
        sampler(UInt2{m_sub_pixels_x,m_sub_pixels_y},&subpixel_samples[0]);

        std::vector<Vec2> emission_samples(m_sub_pixels_x*m_sub_pixels_y);
        
        Vec2* emssion_samples_it = &emission_samples[0];

        Spectrum result = FromValue(0);

        int total_light_samples = m_light_samples_x*m_light_samples_y;

        for(const auto& sample : subpixel_samples)
        {
            std::vector<Vec2> integrator_samples(total_light_samples+1);
            Vec2* samples_it = &integrator_samples[0];
            sampler(UInt2{m_light_samples_x,m_light_samples_y},samples_it);
            integrator_samples[total_light_samples] = *emssion_samples_it;
            Payload payload = {samples_it,FromValue(1)};
            Ray ray = camera(pixel,sample);
            tracer<MATERIAL_BSDF>(ray,&payload);
            result +=payload.result;

            emssion_samples_it++;

        }

        film.SavePixel("Combined",pixel,result/subpixel_samples.size());
    }


    Spectrum ComputeDirectLight(const cts::DistributionFunction auto& bsdf,
                            const cts::DistributionFunction auto& lights,
                            const Vec2& sample,cts::TraceRay auto& tracer)
    {
        Ray light_ray;
        auto[light_val,light_pdf] = lights(&light_ray,sample);
        Payload shadow_test;
        tracer<MATERIAL_TRANSPARENCY>(light_ray,shadow_test);
        if(shadow_test.IsShadow) return Black();
        auto[bsdf_val,bsdf_pdf] = bsdf(light_ray);
        
        if(!IsDeltaDistribution(light_pdf))
            return bsdf_val*light_val/light_pdf;
        return bsdf_val*light_val;
    };


    struct DirectLightBehavior
    {
        void ClosestHitProgram(const cts::HitPoint auto& hit_point,Payload* payload, 
                                cts::TraceRay auto& tracer)
        {
            auto bsdf = hit_point.template Shader<MATERIAL_BSDF>();
            auto lights = hit_point.template ActiveLights();
            auto emission = hit_point.template Shader<MATERIAL_EMISSION>();
            for(int i=0;i<m_light_samples;i++)
            {
                payload->result+= ComputeDirectLight(bsdf, lights, payload->samples[i], tracer);
            }
            payload->result/=m_light_samples;
            payload->result+=emission();
        }
    private:
        uint m_light_samples;        
    };
}