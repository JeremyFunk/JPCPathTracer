#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Vec.h"
#include "tests/test_utils.h"
#include "gtest/gtest.h"

namespace jpctracer::math
{
TEST(math, SpectrumMult)
{
    auto test = Black() * Black();
    EXPECT_TRUE(IsBlack(test));
}

TEST(math, SpectrumMult2)
{
    auto test = FromRGB({1, 1, 1}) * Black();
    EXPECT_TRUE(IsBlack(test));
}

TEST(math, SpectrumMult3)
{
    auto a = FromRGB({2, 3, 2});
    auto test = ToRGB(a);
    TestVec3(test, Vec3{2, 3, 2});
}

TEST(math, SpectrumMult4)
{
    auto b = xyz::FromRGB({1, 1, 1});

    TestVec4(b, Vec<Prec, 4>{0.9505, 1.0000, 1.0890, 1});
}

} // namespace jpctracer::math