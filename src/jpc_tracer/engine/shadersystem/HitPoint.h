#pragma once

#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/shadersystem/IBsdfClosure.h"
#include "jpc_tracer/engine/shadersystem/Lights.h"
#include "jpc_tracer/engine/shadersystem/NormalSpace.h"
#include "jpc_tracer/engine/shadersystem/ShaderFunction.h"
#include "BsdfNode.h"


namespace jpctracer::shadersys
{

    
    class HitPoint
    {
    public:
        HitPoint(const shadersys::IShader* shader,const shadersys::Lights* lights, const NormalSpace& normal_space)
            : m_shader(shader),m_normal_space(normal_space), m_lights(lights)
        {

        }
        //Rays should_be in normal space
        template<MaterialType type>
        ShaderResults Shader(View<Ray> eval_rays,View<Vec2> samples) const
        {
            const Vec3& scat_dir = m_normal_space.ScatteringDir;
            switch (type) {
                case MATERIAL_DIFFUSE:          return m_shader->SampleDIFFUSE       (scat_dir,eval_rays, samples);
                case MATERIAL_GLOSSY:           return m_shader->SampleGLOSSY        (scat_dir,eval_rays, samples);
                case MATERIAL_TRANSMISSION:     return m_shader->SampleTRANSMISSION  (scat_dir,eval_rays, samples);
                case MATERIAL_TRANSPARENCY:     return m_shader->SampleTRANSPARENCY  (scat_dir,eval_rays, samples);
                case MATERIAL_SUBSURFACE:       return m_shader->SampleSUBSURFACE    (scat_dir,eval_rays, samples);
                case MATERIAL_EMISSION:         return m_shader->SampleEMISSION      (scat_dir,eval_rays, samples);
                
                default:                        return m_shader->SampleBSDF          (scat_dir, eval_rays, samples);
            }
        }

        template<MaterialType type>
        ShaderResults Shader(View<Ray> eval_rays) const
        {
            const Vec3& scat_dir = m_normal_space.ScatteringDir;
            switch (type) {
                case MATERIAL_DIFFUSE:          return m_shader->EvalDIFFUSE         (scat_dir, eval_rays);
                case MATERIAL_GLOSSY:           return m_shader->EvalGLOSSY          (scat_dir, eval_rays);
                case MATERIAL_TRANSMISSION:     return m_shader->EvalTRANSMISSION    (scat_dir, eval_rays);
                case MATERIAL_TRANSPARENCY:     return m_shader->EvalTRANSPARENCY    (scat_dir, eval_rays);
                case MATERIAL_SUBSURFACE:       return m_shader->EvalSUBSURFACE      (scat_dir, eval_rays);
                case MATERIAL_EMISSION:         return m_shader->EvalEMISSION        (scat_dir, eval_rays);
                
                default:                        return m_shader->EvalBSDF            (scat_dir, eval_rays);
            }
        }

        Spectrum Emission() const
        {
            const Vec3& scat_dir = m_normal_space.ScatteringDir;
            Ray scat_ray = {scat_dir,{0,0,0}};
            ShaderResults results = m_shader->EvalEMISSION(scat_dir,View<Ray>{&scat_ray,1});
            return results.eval_results[0].luminance;
        }

        ShaderResults ActiveLights(View<Vec2> samples) const
        {
            ShaderResults results = m_lights->Sample(samples, m_normal_space.Interaction);
            for(auto& ray: results.sampled_rays)
                ray = WorldToNormal(ray,m_normal_space);
            return results;
        }
    private:
        const shadersys::IShader* m_shader; 
        const NormalSpace& m_normal_space;
        const shadersys::Lights* m_lights;
        

    };
}