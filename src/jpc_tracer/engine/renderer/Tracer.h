#pragma once
#include "../raytracing/SceneBuilder.h"
#include "jpc_tracer/core/Concepts.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/TraceRay.h"
#include "jpc_tracer/engine/raytracing/detail/TracingContext.h"
#include "jpc_tracer/engine/renderer/ShaderBuffer.h"
#include "jpc_tracer/engine/shadersystem/HitPoint.h"
#include "jpc_tracer/engine/shadersystem/Lights.h"
#include <memory>
#include <optional>

namespace jpctracer::renderer
{
using HitPoint = shadersys::HitPoint;
struct Tracer;
struct IRayBehavior
{
    // returnes if it was an hit

    virtual AnyHitResult AnyHitProgram(const HitPoint& hit_point, Payload* payload) const
    {
        return {true, false};
    }

    virtual void ClosestHitProgram(const HitPoint& hit_point, Payload* payload, Tracer& trace) const
    {
    }

    virtual void Miss(const Spectrum& background_color, Payload* payload) const
    {
    }
};

class Tracer
{

  public:
    inline Tracer(const ShaderBuffer& shader_buffer, const raytracing::Scene* scene, const shadersys::Lights* lights,
                  shadersys::ShaderResultsStack& shader_stack, const Transformation* normal_space)
        : m_shader_buffer(shader_buffer), m_scene(scene), m_normal_space(normal_space), m_lights(lights),
          m_shader_stack(shader_stack)
    {
    }
    inline Tracer(const ShaderBuffer& shader_buffer, const raytracing::Scene* scene, const shadersys::Lights* lights,
                  shadersys::ShaderResultsStack& shader_stack)
        : m_shader_buffer(shader_buffer), m_scene(scene), m_lights(lights), m_shader_stack(shader_stack)
    {
    }

    void operator()(const std::derived_from<IRayBehavior> auto& ray_behavior, Ray& ray, Payload* payload)
    {
        Ray world_ray = ray;
        if (m_normal_space)
        {
            world_ray = TransformBack(*m_normal_space, ray);
        }

        const ShaderBuffer& buffer_temp = m_shader_buffer;
        const shadersys::Lights* lights = m_lights;
        shadersys::ShaderResultsStack& stack = m_shader_stack;

        std::optional<SurfaceInteraction> interaction = raytracing::TraceRay(
            world_ray,
            // anyhit callback
            [&ray_behavior, &payload, &buffer_temp, &lights, &world_ray,
             &stack](const SurfaceInteraction& interaction) -> AnyHitResult {
                const shadersys::IShader* shader = buffer_temp.GetShader(interaction.MaterialId);

                HitPoint hit_point(shader, lights, interaction, world_ray, stack);
                return ray_behavior.AnyHitProgram(hit_point, payload);
            },
            m_scene);

        if (interaction)
        {
            const shadersys::IShader* shader = buffer_temp.GetShader(interaction->MaterialId);
            HitPoint hit_point(shader, lights, *interaction, world_ray, stack);
            Tracer tracer(m_shader_buffer, m_scene, m_lights, stack, &hit_point.normal_space);
            return ray_behavior.ClosestHitProgram(hit_point, payload, tracer);
        }
        else
        {
            return ray_behavior.Miss(Black(), payload);
        }
    }

  private:
    const ShaderBuffer& m_shader_buffer;
    const raytracing::Scene* m_scene;
    const Transformation* m_normal_space = nullptr;
    const shadersys::Lights* m_lights;
    shadersys::ShaderResultsStack& m_shader_stack;
};

} // namespace jpctracer::renderer