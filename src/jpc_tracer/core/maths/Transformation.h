#pragma once
#include "Constants.h"


namespace jpctracer {

    struct Bounds3D
    {
        Vec3 Min;
        Vec3 Max;

        // Bounds3D() : Min({}), Max({})
        // {}

        // Bounds3D(Vec3& min, Vec3& max) : Min(min), Max(max)
        // {}

        Vec3 operator[](const int& i) const
        {
            return (i == 0) ? Min : Max;
        }

        Vec3& operator[](const int& i)
        {
            return (i == 0) ? Min : Max;
        }
    };
}