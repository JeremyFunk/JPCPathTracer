#include "gtest/gtest.h"
#include <iostream>

#include "jpc_tracer/core/maths/Transformation.h"

namespace jpctracer {
    TEST(Bounds, VecTest)
    {
        Vec3 min {0,0,0};
        Vec3 max {2,2,2};

        Bounds3D a {min, max};

        std::cout << a[0][0] << ' ' << a[1][0];
    }
}