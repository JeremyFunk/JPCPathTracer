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

} // namespace jpctracer