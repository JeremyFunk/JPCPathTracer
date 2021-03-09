#pragma once

#include "Vec.h"
#include "Spectrum.h"
namespace jpctracer {
    using Prec = float;
    using Vec3 = math::Vec<Prec,3>;
    using Int3 = math::Vec<int,3>;
    using UInt3 = math::Vec<unsigned int,3>;
    
    using Vec2 = math::Vec<Prec,2>;
    using Int2 = math::Vec<int,2>;
    using UInt2 = math::Vec<unsigned int,2>;
    using Mat3x3 = int;

    using Spectrum = double;

    struct Ray
    {
        Vec3 Direction;
        Vec3 Origin;
        Vec2 LensPosition;
        float Time = 0;
    };

    struct SurfaceInteraction
    {
        Vec3 Point;
        Vec3 Normal;
        Vec2 UV;
        Prec Distance;
        int MaterialId;
    };
}