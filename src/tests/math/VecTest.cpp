#include "gtest/gtest.h"
#include "jpc_tracer/maths/maths.h"

namespace jpctracer::math {
    TEST(math,Vec3Cross)
    {
        Int3 a = {1,2,3};
        Int3 b = {-7,8,9};
        Int3 expected = {-6,-30,22};
        
        Int3 actual = a.cross(b);
        EXPECT_TRUE(actual==expected);
    }

    TEST(math,Vec3Dot)
    {
        Int3 a = {1,2,3};
        Int3 b = {-7,8,9};
        int expected = 1*(-7)+2*8+3*9;
        
        int actual = a.dot(b);
        EXPECT_EQ(expected,actual);
    }
}