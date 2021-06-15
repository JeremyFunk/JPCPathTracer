#include "ShaderResults.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include <OpenImageIO/fmath.h>
#include <cstddef>
namespace jpctracer::shadersys
{
ShaderResultsStack::ShaderResultsStack(uint init_capacity)

{
    m_dists_spec.reserve(init_capacity);
    m_dists_passes.reserve(init_capacity);
    m_rays.reserve(init_capacity);
}

ShaderResultsStack::State ShaderResultsStack::GetCurrentState()
{
    return State{m_dists_spec.size(), m_dists_passes.size(), m_rays.size()};
}

void ShaderResultsStack::SetState(State state)
{
    m_dists_spec.resize(state.bsdf_com_size);
    m_dists_passes.resize(state.bsdf_sep_size);
    m_rays.resize(state.ray_size);
}

bool ShaderResultsStack::IsEmpty()
{
    return m_dists_spec.size() == 0 && m_dists_passes.size() == 0 && m_rays.size() == 0;
}

ShaderResultsCom ShaderResultsStack::CreateCombined(uint eval_count, uint samples_count)
{
    return Setup(eval_count, samples_count, m_dists_spec, FromValue(0));
}

ShaderResultsSep ShaderResultsStack::CreateSeperated(uint eval_count, uint samples_count)
{
    return Setup(eval_count, samples_count, m_dists_passes, Passes{});
}

LightResults ShaderResultsStack::CreateLightResults(uint count)
{
    size_t bsdf_start = m_dists_spec.size();
    for (int i = 0; i < count; i++)
        m_dists_spec.push_back({FromValue(0), 0});
    size_t rays_start = m_rays.size();
    for (int i = 0; i < count; i++)
        m_rays.push_back(Ray{});
    LightResults result;
    result.emission = {&m_dists_spec[bsdf_start], (size_t)count};
    result.rays = {&m_rays[rays_start], (size_t)count};
    return result;
}

template <class T>
ShaderResults<T> ShaderResultsStack::Setup(uint eval_count, uint samples_count, std::vector<Distributed<T>>& bsdf_mem,
                                           T default_val)
{
    ShaderResults<T> result;
    size_t bsdf_eval_start = bsdf_mem.size();
    size_t bsdf_smp_start = bsdf_eval_start + eval_count;
    for (int i = 0; i < eval_count + samples_count; i++)
        bsdf_mem.push_back(Distributed<T>{default_val, 0});

    result.eval_bsdf = {bsdf_mem.data() + bsdf_eval_start, eval_count};
    result.sampled_bsdf = {bsdf_mem.data() + bsdf_smp_start, samples_count};

    result.emission = Black();
    result.transparency = 1;

    // rays
    size_t rays_start = m_rays.size();
    for (int i = 0; i < samples_count; i++)
        m_rays.push_back(Ray{});

    result.sampled_rays = {m_rays.data() + rays_start, samples_count};
    return result;
}
} // namespace jpctracer::shadersys