#pragma once

#include "jpc_tracer/core/core.h"
#include "gtest/gtest.h"

namespace jpctracer {
    inline void TestVec3(const Vec3& a,const Vec3& b)
    {
        EXPECT_NEAR(a[0],b[0],0.001);
        EXPECT_NEAR(a[1],b[1],0.001);
        EXPECT_NEAR(a[2],b[2],0.001);
        
    }
}
