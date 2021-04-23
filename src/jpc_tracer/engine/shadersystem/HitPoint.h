#pragma once

#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/shadersystem/Lights.h"
#include "jpc_tracer/engine/shadersystem/NormalSpace.h"
#include "jpc_tracer/engine/shadersystem/Shader.h"

namespace jpctracer::shadersys
{

class HitPoint
{

  public:
    HitPoint(const shadersys::IShader* shader, const shadersys::Lights* lights, const NormalSpace& normal_space,
             ShaderResultsStack& results_stack)
        : m_shader(shader), m_normal_space(normal_space), m_lights(lights), m_results_stack(results_stack)
    {
        m_stack_begin_state = m_results_stack.GetCurrentState();
    }
    // Rays should_be in normal space
    CombinedBsdfs Shader(View<Ray> eval_rays, View<Vec2> samples) const
    {
        CombinedBsdfs result = m_results_stack.CreateCombined(eval_rays.size, samples.size);
        m_shader->Sample(m_normal_space.Interaction, m_normal_space.ScatteringRay, eval_rays, samples, result);
        return result;
    }

    CombinedBsdfs Shader(View<Ray> eval_rays) const
    {
        CombinedBsdfs result = m_results_stack.CreateCombined(eval_rays.size, 0);
        m_shader->Eval(m_normal_space.Interaction, m_normal_space.ScatteringRay, eval_rays, result);
        return result;
    }

    SeperatedBsdfs ShaderSeperated(View<Ray> eval_rays, View<Vec2> samples) const
    {
        SeperatedBsdfs result = m_results_stack.CreateSeperated(eval_rays.size, samples.size);
        m_shader->Sample(m_normal_space.Interaction, m_normal_space.ScatteringRay, eval_rays, samples, result);
        return result;
    }

    SeperatedBsdfs ShaderSeperated(View<Ray> eval_rays) const
    {
        SeperatedBsdfs result = m_results_stack.CreateSeperated(eval_rays.size, 0);
        m_shader->Eval(m_normal_space.Interaction, m_normal_space.ScatteringRay, eval_rays, result);
        return result;
    }

    Spectrum Emission() const
    {
        return m_shader->Emission(m_normal_space.Interaction);
    }

    Prec Transparency() const
    {
        return m_shader->Transparency(m_normal_space.Interaction);
    }

    LightResults ActiveLights(View<Vec2> samples) const
    {
        LightResults result = m_results_stack.CreateLightResults(samples.size);

        m_lights->Sample(samples, m_normal_space.Interaction, result);
        for (auto& ray : result.rays)
            ray = WorldToNormal(ray, m_normal_space);
        return result;
    }
    ~HitPoint()
    {
        m_results_stack.SetState(m_stack_begin_state);
    }

  private:
    const shadersys::IShader* m_shader;
    const NormalSpace& m_normal_space;
    const shadersys::Lights* m_lights;
    ShaderResultsStack& m_results_stack;
    ShaderResultsStack::State m_stack_begin_state;
};
} // namespace jpctracer::shadersys