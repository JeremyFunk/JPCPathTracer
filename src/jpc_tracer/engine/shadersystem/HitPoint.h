#pragma once

#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/shadersystem/Lights.h"
#include "jpc_tracer/engine/shadersystem/Shader.h"
#include "jpc_tracer/engine/shadersystem/ShaderResults.h"

namespace jpctracer::shadersys
{

class HitPoint
{

  public:
    HitPoint(const IShader* shader, const Lights* lights, const SurfaceInteraction& _interaction, const Ray& world_ray,
             ShaderResultsStack& results_stack)
        : m_shader(shader), m_lights(lights), m_results_stack(results_stack),
          normal_space(CreateNormalSpace(_interaction.Normal, _interaction.Point)), interaction(_interaction)
    {
        m_stack_begin_state = m_results_stack.GetCurrentState();
        scattered_ray = TransformTo(normal_space, world_ray);
        if (scattered_ray.Direction[2] < 0)
            scattered_ray.Direction.flip();
    }
    // Rays should_be in normal space
    ShaderResultsCom Shader(View<Ray> eval_rays, View<Vec2> samples) const
    {
        auto result = m_results_stack.CreateCombined(eval_rays.size, samples.size);
        m_shader->Sample(interaction, scattered_ray, eval_rays, samples, result);
        return result;
    }

    ShaderResultsCom Shader(View<Ray> eval_rays) const
    {
        auto result = m_results_stack.CreateCombined(eval_rays.size, 0);
        m_shader->Eval(interaction, scattered_ray, eval_rays, result);
        return result;
    }

    ShaderResultsSep ShaderSeperated(View<Ray> eval_rays, View<Vec2> samples) const
    {
        auto result = m_results_stack.CreateSeperated(eval_rays.size, samples.size);
        m_shader->Sample(interaction, scattered_ray, eval_rays, samples, result);
        return result;
    }

    ShaderResultsSep ShaderSeperated(View<Ray> eval_rays) const
    {
        auto result = m_results_stack.CreateSeperated(eval_rays.size, 0);
        m_shader->Eval(interaction, scattered_ray, eval_rays, result);
        return result;
    }

    Spectrum Emission() const
    {
        return m_shader->Emission(interaction);
    }

    Prec Transparency() const
    {
        return m_shader->Transparency(interaction);
    }

    LightResults ActiveLights(View<Vec2> samples) const
    {
        LightResults result = m_results_stack.CreateLightResults(samples.size);

        m_lights->Sample(samples, interaction, result);
        for (auto& ray : result.rays)
            ray = TransformBack(normal_space, ray);
        return result;
    }
    ~HitPoint()
    {
        m_results_stack.SetState(m_stack_begin_state);
    }

    const Transformation normal_space;
    // in wolrd space
    const SurfaceInteraction& interaction;
    // in normalspace
    Ray scattered_ray;

  private:
    const IShader* m_shader;
    const Lights* m_lights;
    ShaderResultsStack& m_results_stack;
    ShaderResultsStack::State m_stack_begin_state;
};
} // namespace jpctracer::shadersys