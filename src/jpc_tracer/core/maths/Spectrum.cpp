#include "Spectrum.h"
#include "jpc_tracer/core/maths/Vec.h"
#include <functional>

namespace jpctracer
{

namespace math
{
void operator*=(Spectrum& spec1, const Spectrum& spec2)
{
    math::detail::VecMutableOp(spec1, spec2, std::multiplies<Prec>());
}

Spectrum operator*(const Spectrum& spec1, const Spectrum& spec2)
{
    return math::detail::VecImmutableOp(spec1, spec2, std::multiplies<Prec>());
}
} // namespace math

Prec& Transparency(Spectrum& spec)
{
    // return spec[4];
    return spec[3];
}

bool IsBlack(Spectrum spec)
{
    return std::abs(spec[0]) < 0.0001 && std::abs(spec[1]) < 0.0001 && std::abs(spec[2]) < 0.0001;
}

Spectrum Black()
{
    return Spectrum{0, 0, 0, 1};
}

Spectrum Vec3ToSpec(const Vec3& b)
{
    return Spectrum{b[0] * 0.5f + 0.5f, b[1] * 0.5f + 0.5f, b[2] * 0.5f + 0.5f, 1.f};
}
Spectrum FromValue(const Prec& v)
{
    return Spectrum{v, v, v, v};
}

namespace srgb
{

Vec3 ToRGB(const Spectrum& spec) noexcept
{
    return Vec3{spec[0], spec[1], spec[2]};
}

Spectrum FromRGB(const Vec3& rgb) noexcept
{
    return Spectrum{rgb[0], rgb[1], rgb[2], 1.0};
}
} // namespace srgb

namespace xyz
{

Prec degamma(Prec v) noexcept
{
    if (v <= 0.04045)
        return v / 12.92;
    else
    {
        Prec e = (v + 0.055) / 1.055;
        return std::pow(e, 2.4);
    }
}

Prec gamma(Prec v) noexcept
{
    if (v <= 0.0031308)
        return v * 12.92;
    else
        return 1.055 * std::pow(v, (1. / 2.4)) - 0.055;
}
// clang-format off
/*
static const Prec lrgb_to_xyz_mat[9] = {
    0.4124108464885388,   0.3575845678529519,  0.18045380393360833,
	0.21264934272065283,  0.7151691357059038,  0.07218152157344333,
	0.019331758429150258, 0.11919485595098397, 0.9503900340503373
};
static const Prec xyz_to_lrgb_mat[9] = {
    3.240812398895283,    -1.5373084456298136,  -0.4985865229069666,
	-0.9692430170086407,    1.8759663029085742,   0.04155503085668564,
	0.055638398436112804, -0.20400746093241362,  1.0571295702861434
};*/

static const Prec lrgb_to_xyz_mat[9] = {
    0.4124,   0.3576,  0.1805,
	0.2126,  0.7152,  0.0722,
	0.0193, 0.1192, 0.9505
};
static const Prec xyz_to_lrgb_mat[9] = {
    3.2406,    -1.5372,  -0.4986,
	-0.9689,    1.8758,   0.0415,
	0.0557, -0.2040,  1.0570
};

Vec3 ToRGB(const Spectrum& spec) noexcept
{
    const Prec* M = xyz_to_lrgb_mat;
    const auto& s = spec;
    Vec3 linear_rgb = {
        M[0] * s[0] + M[1] * s[1] + M[2] * s[2],
        M[3] * s[0] + M[4] * s[1] + M[5] * s[2],
        M[6] * s[0] + M[7] * s[1] + M[8] * s[2],
    };
    return {gamma(linear_rgb[0]), gamma(linear_rgb[1]), gamma(linear_rgb[2])};
}

Spectrum FromRGB(const Vec3& rgb) noexcept
{
    const Prec* M = lrgb_to_xyz_mat;
    const Vec3 s = {degamma(rgb[0]), degamma(rgb[1]), degamma(rgb[2])};
    Vec3 xyz = {
        M[0] * s[0] + M[1] * s[1] + M[2] * s[2],
        M[3] * s[0] + M[4] * s[1] + M[5] * s[2],
        M[6] * s[0] + M[7] * s[1] + M[8] * s[2]
    };
    return Spectrum{xyz[0], xyz[1], xyz[2], 1};
}
// clang-format on
} // namespace xyz
} // namespace jpctracer