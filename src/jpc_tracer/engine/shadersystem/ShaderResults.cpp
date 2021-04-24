#include "ShaderResults.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include <OpenImageIO/fmath.h>
#include <cstddef>
namespace jpctracer::shadersys
{
ShaderResultsStack::ShaderResultsStack(uint init_capacity)

{
    m_bsdf_com.reserve(init_capacity);
    m_bsdf_sep.reserve(init_capacity);
    m_pdfs.reserve(init_capacity);
    m_rays.reserve(init_capacity);
}

ShaderResultsStack::State ShaderResultsStack::GetCurrentState()
{
    return State{m_bsdf_com.size(), m_bsdf_sep.size(), m_pdfs.size(), m_rays.size()};
}

void ShaderResultsStack::SetState(State state)
{
    m_bsdf_com.resize(state.bsdf_com_size);
    m_bsdf_sep.resize(state.bsdf_sep_size);
    m_pdfs.resize(state.pdf_size);
    m_rays.resize(state.ray_size);
}

bool ShaderResultsStack::IsEmpty()
{
    return m_bsdf_com.size() == 0 && m_bsdf_sep.size() == 0 && m_pdfs.size() == 0 && m_rays.size() == 0;
}

CombinedBsdfs ShaderResultsStack::CreateCombined(uint eval_count, uint samples_count)
{
    size_t bsdf_start = m_bsdf_com.size();
    size_t bsdf_eval_end = bsdf_start + eval_count;
    size_t bsdf_samples_end = bsdf_eval_end + samples_count;

    for (int i = 0; i < eval_count + samples_count; i++)
        m_bsdf_com.push_back(FromValue(0));

    CombinedBsdfs result;
    result.all_bsdfs = {&m_bsdf_com[bsdf_start], (size_t)eval_count + samples_count};
    result.eval_bsdfs = {&m_bsdf_com[bsdf_start], eval_count};
    if (samples_count != 0)
        result.sampled_bsdfs = {&m_bsdf_com[bsdf_eval_end], samples_count};
    else
        result.sampled_bsdfs = {nullptr, samples_count};
    Setup(eval_count, samples_count, result);
    return result;
}

SeperatedBsdfs ShaderResultsStack::CreateSeperated(uint eval_count, uint samples_count)
{
    size_t bsdf_start = m_bsdf_sep.size();
    size_t bsdf_eval_end = bsdf_start + eval_count;
    size_t bsdf_samples_end = bsdf_eval_end + samples_count;

    for (int i = 0; i < eval_count + samples_count; i++)
        m_bsdf_sep.push_back(LightPasses{});

    SeperatedBsdfs result;
    result.all_bsdfs = {&m_bsdf_sep[bsdf_start], (size_t)eval_count + samples_count};
    result.eval_bsdfs = {&m_bsdf_sep[bsdf_start], eval_count};
    if (samples_count != 0)
        result.sampled_bsdfs = {&m_bsdf_sep[bsdf_eval_end], samples_count};
    else
        result.sampled_bsdfs = {nullptr, samples_count};
    Setup(eval_count, samples_count, result);
    return result;
}

LightResults ShaderResultsStack::CreateLightResults(uint count)
{
    size_t bsdf_start = m_bsdf_com.size();
    for (int i = 0; i < count; i++)
        m_bsdf_com.push_back(FromValue(0));
    size_t pdf_start = m_pdfs.size();
    for (int i = 0; i < count; i++)
        m_pdfs.push_back(0);
    size_t rays_start = m_rays.size();
    for (int i = 0; i < count; i++)
        m_rays.push_back(Ray{});
    LightResults result;
    result.emission = {&m_bsdf_com[bsdf_start], (size_t)count};
    result.pdf = {&m_pdfs[pdf_start], (size_t)count};
    result.rays = {&m_rays[rays_start], (size_t)count};
    return result;
}

template <class T> void ShaderResultsStack::Setup(uint eval_count, uint samples_count, T& result)
{
    size_t pdf_start = m_pdfs.size();
    size_t pdf_eval_end = pdf_start + eval_count;
    size_t pdf_samples_end = pdf_eval_end + samples_count;
    for (int i = 0; i < eval_count + samples_count; i++)
        m_pdfs.push_back(0);

    size_t rays_start = m_rays.size();
    for (int i = 0; i < samples_count; i++)
        m_rays.push_back(Ray{});

    result.all_pdf = {&m_pdfs[pdf_start], (size_t)eval_count + samples_count};
    result.eval_pdf = {&m_pdfs[pdf_start], eval_count};

    if (samples_count != 0)
        result.sampled_pdf = {&m_pdfs[pdf_eval_end], samples_count};
    else
            result.sampled_pdf = {nullptr, samples_count};

    result.emission = Black();
    result.transparency = 1;
    if (samples_count != 0)
        result.sampled_rays = {&m_rays[rays_start], samples_count};
    else
        result.sampled_rays = {nullptr, samples_count};
}
} // namespace jpctracer::shadersys