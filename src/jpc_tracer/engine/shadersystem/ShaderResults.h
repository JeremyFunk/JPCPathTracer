#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include <vector>

namespace jpctracer::shadersys
{

struct LightPasses
{
    Spectrum diffuse = FromValue(0);
    Spectrum glossy = FromValue(0);
    Spectrum transmission = FromValue(0);
    Spectrum transparent = FromValue(0);
    Spectrum subsurface = FromValue(0);
    Spectrum scatter = FromValue(0);
};

struct SeperatedBsdfs
{
    Spectrum emission;
    Prec transparency;

    View<LightPasses> eval_bsdfs;
    View<LightPasses> sampled_bsdfs;
    View<LightPasses> all_bsdfs;
    View<Ray> sampled_rays;

    View<Prec> eval_pdf;
    View<Prec> sampled_pdf;
    View<Prec> all_pdf;
};

struct CombinedBsdfs
{
    Spectrum emission;
    Prec transparency;

    View<Spectrum> eval_bsdfs;
    View<Spectrum> sampled_bsdfs;
    View<Spectrum> all_bsdfs;
    View<Ray> sampled_rays;

    View<Prec> eval_pdf;
    View<Prec> sampled_pdf;
    View<Prec> all_pdf;
};

struct LightResults
{
    View<Prec> pdf;
    View<Spectrum> emission;
    View<Ray> rays;
};

class ShaderResultsStack
{
  public:
    ShaderResultsStack(uint init_capacity = 200);

    struct State
    {
        size_t bsdf_sep_size;
        size_t bsdf_com_size;
        size_t pdf_size;
        size_t ray_size;
    };

    State GetCurrentState();
    void SetState(State state);
    bool IsEmpty();

    CombinedBsdfs CreateCombined(uint eval_count, uint samples_count);
    SeperatedBsdfs CreateSeperated(uint eval_count, uint samples_count);
    LightResults CreateLightResults(uint count);

  private:
    template <class T> void Setup(uint eval_count, uint samples_count, T& result);
    std::vector<LightPasses> m_bsdf_sep;
    std::vector<Spectrum> m_bsdf_com;
    std::vector<Prec> m_pdfs;
    std::vector<Ray> m_rays;
};

} // namespace jpctracer::shadersys