#pragma once
#include "Constants.h"


namespace jpctracer {

    struct Transformation
    {
        Mat3x3 RotationScale;
        Vec3 Translation;
    };

    struct Bounds3D
    {
        Vec3 Min;
        Vec3 Max;
    };
}