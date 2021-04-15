#pragma once
#include "../raytracing/SceneBuilder.h"
#include "jpc_tracer/core/Concepts.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "../shadersystem/HitPoint.h"
#include "jpc_tracer/engine/raytracing/TraceRay.h"
#include "jpc_tracer/engine/raytracing/detail/TracingContext.h"
#include "jpc_tracer/engine/renderer/ShaderBuffer.h"
#include "jpc_tracer/engine/shadersystem/BsdfNode.h"
#include "jpc_tracer/engine/shadersystem/Lights.h"
#include "jpc_tracer/engine/shadersystem/NormalSpace.h"
#include "jpc_tracer/engine/shadersystem/HitPoint.h"
#include <memory>
#include <optional>


namespace jpctracer::renderer
{
    using HitPoint = shadersys::HitPoint;
    using NormalSpace = shadersys::NormalSpace;
    struct Tracer;
    struct IRayBehavior
    {
        //returnes if it was an hit

        virtual AnyHitResult AnyHitProgram(const HitPoint& hit_point,Payload* payload) const
        {
            return {true,false};
        }

        virtual void ClosestHitProgram(const HitPoint& hit_point, Payload* payload ,Tracer& trace) const
        {}


        virtual void Miss(const Spectrum& background_color, Payload* payload) const
        {}
    };
    
    class Tracer
    {
    
    public:

        inline Tracer(const ShaderBuffer& shader_buffer, const raytracing::Scene* scene,
                const shadersys::Lights* lights, const NormalSpace& normal_space)
            : m_shader_buffer(shader_buffer),m_scene(scene),m_normal_space(&normal_space),m_lights(lights)
            {}
        inline Tracer(const ShaderBuffer& shader_buffer, const raytracing::Scene* scene,
                const shadersys::Lights* lights)
            : m_shader_buffer(shader_buffer),m_scene(scene),m_lights(lights)
            {}
        
        void operator()(const std::derived_from<IRayBehavior> auto& ray_behavior,const Ray& ray,Payload* payload)
        {
            Ray world_ray = ray;
            if(m_normal_space)
                Ray world_ray = NormalToWorld(ray,*m_normal_space);

            const ShaderBuffer& buffer_temp = m_shader_buffer;
            const shadersys::Lights* lights = m_lights;

            std::optional<SurfaceInteraction> interaction = raytracing::TraceRay(ray, 
                //anyhit callback
                [&ray_behavior,&payload,&buffer_temp,&lights,&world_ray](const SurfaceInteraction& interaction) ->AnyHitResult
                {
                    const shadersys::IShader* shader = buffer_temp.GetShader(interaction.MaterialId);

                    HitPoint hit_point(shader,lights,shadersys::CreateNormalSpace(world_ray, interaction));
                    return ray_behavior.AnyHitProgram(hit_point,payload);
                }, 
                m_scene);

            if(interaction)
            {
                const shadersys::IShader* shader = buffer_temp.GetShader(interaction->MaterialId);
                auto new_normal_space = shadersys::CreateNormalSpace(world_ray, *interaction);
                HitPoint hit_point(shader,lights,new_normal_space);
                Tracer tracer(m_shader_buffer,m_scene,m_lights,new_normal_space);
                return ray_behavior.ClosestHitProgram(hit_point,payload,tracer);
            }else
            {
                return ray_behavior.Miss(Black(),payload);
            }


        }
    private:
        const ShaderBuffer& m_shader_buffer;
        const raytracing::Scene* m_scene;
        const NormalSpace* m_normal_space = nullptr;
        const shadersys::Lights* m_lights;
        

    };

    
}