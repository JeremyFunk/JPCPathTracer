#pragma once
#include"Base.h"
#include "Linalg.h"

namespace jpc_rt_core {
    class SurfaceInteraction
    {
    public:
        Vec3 Point;
        Vec3 Normal;
        Vec2 UV;
        Prec Distance;
    };
}

