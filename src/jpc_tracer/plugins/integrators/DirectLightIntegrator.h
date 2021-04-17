#pragma once
#include "jpc_tracer/core/Concepts.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "Payload.h"
#include <vector>
#include "ShadowBehavior.h"
#include "jpc_tracer/engine/PluginsApi.h"

namespace jpctracer
{
    struct DirectLightIntegratorSettings
    {
        uint LightSamples;
        uint SubPixels;
    };



    inline Spectrum ComputeDirectLight(const  ShaderResult& bsdf,
                            const ShaderResult& light,
                            Ray& ray,
                            Tracer& tracer,
                            const ShadowBehavior& shadow_behavior )
    {

        
        Payload shadow_test;
        tracer(shadow_behavior,ray,&shadow_test);
        if(shadow_test.IsShadow) return Black();

        auto[light_val,light_pdf] = light;
        auto[bsdf_val,bsdf_pdf] = bsdf;
        if (!IsDeltaDistribution(light_pdf))
            return bsdf_val * light_val / light_pdf *std::abs(CosTheta(ray.Direction));
        return bsdf_val * light_val * std::abs(CosTheta(ray.Direction));
    };


    struct DirectLightBehavior final : public IRayBehavior
    {
        DirectLightBehavior(uint light_samples);
        inline void ClosestHitProgram(const HitPoint& hit_point, Payload* payload ,Tracer& tracer) const
        {
            
            ShaderResults lights = hit_point.ActiveLights({payload->samples,m_light_samples});
            payload->samples+=m_light_samples;
            
            ShaderResults bsdfs = hit_point.Shader<MATERIAL_BSDF>(lights.sampled_rays);
            //payload->samples+=1;

            for(int i=0;i<m_light_samples;i++)
            {
                payload->result+= ComputeDirectLight(bsdfs.eval_results[i], lights.sampled_results[i], lights.sampled_rays[i], tracer, m_shadow_behavior);
            }
            payload->result/=m_light_samples;
            payload->result+= bsdfs.emission;
        }

        inline void Miss(Spectrum background_color,Payload* payload) const 
        {
            payload->result=background_color;
        }
    private:
        uint m_light_samples; 
        ShadowBehavior m_shadow_behavior;       
    };

    class DirectLightIntegrator final: public IIntegrator
    {
    public:
        DirectLightIntegrator(uint sub_pixels,uint light_samples);
        void Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler,
            Tracer& tracer, film::Film& film) const;
        
    private:
        uint m_sub_pixels_x;
        uint m_sub_pixels_y;
        uint m_light_samples_x;
        uint m_light_samples_y;
        DirectLightBehavior direct_light_behavior;
        
        

    };
    
}