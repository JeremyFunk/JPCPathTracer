#pragma once
#include "Mat.h"
#include "Vec.h"
#include <cstdint>

namespace jpctracer
{
using uint = std::uint16_t;
using Prec = float;
using Vec3 = math::Vec<Prec, 3>;
using Int3 = math::Vec<int, 3>;
using UInt3 = math::Vec<unsigned int, 3>;
using Norm3 = math::Norm<Prec, 3>;

using Vec2 = math::Vec<Prec, 2>;
using Int2 = math::Vec<int, 2>;
using UInt2 = math::Vec<unsigned int, 2>;
using Mat3x3 = math::Mat<Prec, 3, 3>;
using Mat4x4 = math::Mat<Prec, 4, 4>;

struct Ray
{
    Norm3 Direction;
    Vec3 Origin;
    Vec2 LensPosition = {0, 0};
    Prec ClipEnd = 10000;
    float Time = 0;
};

struct SurfaceInteraction
{
    Vec3 Point;
    Norm3 Normal;
    Vec2 UV;
    Prec Distance;
    int MaterialId;
};

#ifdef _MSC_VER
#define M_E 2.71828182845904523536        // e
#define M_LOG2E 1.44269504088896340736    // log2(e)
#define M_LOG10E 0.434294481903251827651  // log10(e)
#define M_LN2 0.693147180559945309417     // ln(2)
#define M_LN10 2.30258509299404568402     // ln(10)
#define M_PI 3.14159265358979323846       // pi
#define M_PI_2 1.57079632679489661923     // pi/2
#define M_PI_4 0.785398163397448309616    // pi/4
#define M_1_PI 0.318309886183790671538    // 1/pi
#define M_2_PI 0.636619772367581343076    // 2/pi
#define M_2_SQRTPI 1.12837916709551257390 // 2/sqrt(pi)
#define M_SQRT2 1.41421356237309504880    // sqrt(2)
#define M_SQRT1_2 0.707106781186547524401 // 1/sqrt(2)
#endif

} // namespace jpctracer