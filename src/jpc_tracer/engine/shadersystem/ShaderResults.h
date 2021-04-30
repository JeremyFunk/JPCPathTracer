#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include <vector>

namespace jpctracer::shadersys
{

struct Passes
{
    Spectrum diffuse = FromValue(0);
    Spectrum glossy = FromValue(0);
    Spectrum transmission = FromValue(0);
    Spectrum subsurface = FromValue(0);
};

template <class T> struct Distributed
{
    T value;
    Prec pdf;
};

template <class T> struct ShaderResults
{
    Spectrum emission;
    Prec transparency;

    View<Ray> sampled_rays;
    View<Distributed<T>> eval_bsdf;
    View<Distributed<T>> sampled_bsdf;
};

using ShaderResultsCom = ShaderResults<Spectrum>;
using ShaderResultsSep = ShaderResults<Passes>;

struct LightResults
{
    View<Distributed<Spectrum>> emission;
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
        size_t ray_size;
    };

    State GetCurrentState();
    void SetState(State state);
    bool IsEmpty();

    ShaderResults<Spectrum> CreateCombined(uint eval_count, uint samples_count);
    ShaderResults<Passes> CreateSeperated(uint eval_count, uint samples_count);
    LightResults CreateLightResults(uint count);

  private:
    template <class T>
    ShaderResults<T> Setup(uint eval_count, uint samples_count, std::vector<Distributed<T>>& bsdf_mem, T default_val);
    std::vector<Distributed<Passes>> m_dists_passes;
    std::vector<Distributed<Spectrum>> m_dists_spec;
    std::vector<Ray> m_rays;
};

/******************************************************************************/
/*                        Operators                                           */
/******************************************************************************/

// clang-format off

inline Passes operator+(const Passes& a, const Passes& b){
    return
    {
        a.diffuse       + b.diffuse,
        a.glossy        + b.glossy,
        a.transmission  + b.transmission,
        a.subsurface    + b.subsurface
    };
}
inline Passes operator-(const Passes& a, const Passes& b){
    return
    {
        a.diffuse       - b.diffuse,
        a.glossy        - b.glossy,
        a.transmission  - b.transmission,
        a.subsurface    - b.subsurface
    };
}
inline Passes operator*(const Passes& a, const Passes& b){
    return
    {
        a.diffuse       * b.diffuse,
        a.glossy        * b.glossy,
        a.transmission  * b.transmission,
        a.subsurface    * b.subsurface
    };
}
template<std::convertible_to<Prec> T>
inline Passes operator*(const Passes& a, const T& b){
    return
    {
        a.diffuse       * b,
        a.glossy        * b,
        a.transmission  * b,
        a.subsurface    * b
    };
}

inline Passes operator*(const Passes& a, const Spectrum& b){
    return
    {
        a.diffuse       * b,
        a.glossy        * b,
        a.transmission  * b,
        a.subsurface    * b
    };
}


inline void operator+=(Passes& a, const Passes& b){

    a.diffuse       += b.diffuse;
    a.glossy        += b.glossy;
    a.transmission  += b.transmission;
    a.subsurface    += b.subsurface;
}

inline void operator-=(Passes& a, const Passes& b){

    a.diffuse       -= b.diffuse;
    a.glossy        -= b.glossy;
    a.transmission  -= b.transmission;
    a.subsurface    -= b.subsurface;
}

inline void operator*=(Passes& a, const Passes& b){

    a.diffuse       *= b.diffuse;
    a.glossy        *= b.glossy;
    a.transmission  *= b.transmission;
    a.subsurface    *= b.subsurface;
}

// clang-format on

} // namespace jpctracer::shadersys