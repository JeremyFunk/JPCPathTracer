#pragma once
#include "Linalg.h"

namespace core {
    class SurfaceInteraction
    {
    public:
        core::Vec3 Point;
        core::Vec3 Normal;
        core::Vec2 UV;
        core::Prec Distance;
    };
}

