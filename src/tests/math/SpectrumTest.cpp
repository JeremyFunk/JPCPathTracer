#include "gtest/gtest.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "tests/test_utils.h"

namespace jpctracer::math {
    TEST(math,SpectrumMult)
    {
        auto test = Black() * Black(); 
        EXPECT_TRUE(IsBlack(test));
    }

    TEST(math,SpectrumMult2)
    {
        auto test = FromRGB({1,1,1}) * Black(); 
        EXPECT_TRUE(IsBlack(test));
    }


    TEST(math,SpectrumMult3)
    {
        auto test_spec = FromRGB({1,1,1}) * FromRGB({2,3,2}); 
        auto test = ToRGB(test_spec);
        TestVec3(test, Vec3{2,3,2});
    }
}