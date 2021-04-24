#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "gtest/gtest.h"

namespace jpctracer::math
{
TEST(math, Mat3x3dotVec3)
{
    // clang-format off
    Mat3x3 mat = {1,2,3,
                    4,5,6,
                    7,8,9}; 

    Vec3 vec = {4,5,6};

    Vec3 res = mat.dot(vec);

    Vec3 comp = {14+18, 4*4+5*5+6*6, 4*7+5*8+6*9};

    for(int i = 0; i < 3; i++)
        EXPECT_EQ(res[i], comp[i]);
}

TEST(math, Mat3x3dotMat3x3)
{
    Mat3x3 mat1 = {1,2,3,
                    4,5,6,
                    7,8,9};

    Mat3x3 mat2 = {2,0,1,
                    0,2,0,
                    1,0,2};

    auto res = mat1.dot(mat2);

    Mat3x3 comp = { 5,  4,  7,
                    14, 10, 16,
                    23, 16, 25};

    for(int i = 0; i < 9; i++)
        EXPECT_EQ(res[i], comp[i]);
}
// clang-format on
} // namespace jpctracer::math