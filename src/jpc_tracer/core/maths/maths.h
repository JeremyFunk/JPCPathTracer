#pragma once

#include "Vec.h"
#include "Spectrum.h"
#include "Shapes.h"
#include "Transformation.h"
#include "Constants.h"
#include "Spectrum.h"
namespace jpctracer {


    struct Ray
    {
        Vec3 Direction;
        Vec3 Origin;
        Vec2 LensPosition = {0,0};
        Prec ClipEnd = 10000;
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