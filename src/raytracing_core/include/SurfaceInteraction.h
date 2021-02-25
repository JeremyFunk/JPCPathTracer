#pragma once
#include"Base.h"

namespace jpc_rt_core {
    struct SurfaceInteraction
    {
        Vec3 Point;
        Vec3 Normal;
        Vec2 UV;
        Prec Distance;
        int MaterialId;
    };
}

