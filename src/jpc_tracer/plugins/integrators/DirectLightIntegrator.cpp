#include "DirectLightIntegrator.h"
#include <math.h>



namespace jpctracer
{
    DirectLightBehavior::DirectLightBehavior(uint light_samples) 
        : m_light_samples(light_samples)
    {
        
    }

    DirectLightIntegrator::DirectLightIntegrator(uint sub_pixels,uint light_samples) 
        : direct_light_behavior(0)
    {
        m_sub_pixels_x = std::ceil(std::sqrt(sub_pixels));
        m_sub_pixels_y = m_sub_pixels_x;
        m_light_samples_x = std::ceil(std::sqrt(light_samples));
        m_light_samples_y = m_light_samples_x;
        direct_light_behavior = DirectLightBehavior(m_light_samples_x*m_light_samples_y);
        
    }

    void DirectLightIntegrator::Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler,
                Tracer& tracer, film::Film& film) const
    {

        std::vector<Vec2> subpixel_samples(m_sub_pixels_x*m_sub_pixels_y);
        sampler->Sample(UInt2{m_sub_pixels_x,m_sub_pixels_y},&subpixel_samples[0]);

        std::vector<Vec2> emission_samples(m_sub_pixels_x*m_sub_pixels_y);
        
        Vec2* emssion_samples_it = &emission_samples[0];

        Spectrum result = FromValue(0);

        int total_light_samples = m_light_samples_x*m_light_samples_y;

        for(const auto& sample : subpixel_samples)
        {
            std::vector<Vec2> integrator_samples(total_light_samples+1);
            Vec2* samples_it = &integrator_samples[0];
            sampler->Sample(UInt2{m_light_samples_x,m_light_samples_y},samples_it);
            integrator_samples[total_light_samples] = *emssion_samples_it;
            Payload payload = {samples_it,FromValue(1)};

            Ray ray = camera->Generate(pixel,sample);
            tracer(direct_light_behavior,ray,&payload);
            result +=payload.result;

            emssion_samples_it++;
        }

        

        film.SavePixel("Combined",pixel,result/subpixel_samples.size());
    }
}