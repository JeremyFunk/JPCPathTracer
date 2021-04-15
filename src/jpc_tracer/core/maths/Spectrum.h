#pragma once

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Vec.h"
namespace jpctracer
{
    using Spectrum = math::Vec<Prec,4>;

    Vec3 ToRGB(const Spectrum& spec);
    constexpr Spectrum FromRGB(const Vec3& rgb) noexcept
    {
        return Spectrum{rgb[0],rgb[1],rgb[2],1.0};
    }
    Prec& Transparency(Spectrum& spec);
    

    bool IsBlack(Spectrum spec);
    Spectrum Black();
    Spectrum FromValue(const Prec& v);

    Spectrum Vec3ToSpec(const Vec3& b);

    namespace math {

        Spectrum operator*(const Spectrum& spec1,const Spectrum& spec2);

        void operator*=(Spectrum& spec1,const Spectrum& spec2);
    }
    

}