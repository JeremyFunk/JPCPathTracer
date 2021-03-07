#pragma once
#include"Base.h"

namespace jpctracer::raytracing {
    struct SurfaceInteraction
    {
        Vec3 Point;
        Vec3 Normal;
        Vec2 UV;
        Prec Distance;
        int MaterialId;
    };
}

