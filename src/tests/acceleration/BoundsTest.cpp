#include "gtest/gtest.h"
#include <iostream>

#include "jpc_tracer/core/maths/Transformation.h"

namespace jpctracer
{
TEST(Bounds, VecTest)
{
    Vec3 min{0, 0, 0};
    Vec3 max{2, 2, 2};

    Bounds3D a{min, max};

    EXPECT_EQ(a[0][0], 0);
    EXPECT_EQ(a[1][0], 2);
}
} // namespace jpctracer