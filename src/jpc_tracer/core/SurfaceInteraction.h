#pragma once
#include "Linalg.h"

namespace jpc_tracer {
    class SurfaceInteraction
    {
    public:
        Vec3 Point;
        Vec3 Normal;
        Vec2 UV;
        Prec Distance;
    };
}

