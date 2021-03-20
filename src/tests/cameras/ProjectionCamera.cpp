#include "gtest/gtest.h"
#include "jpc_tracer/plugins/cameras/cameras.h"
#include "../test_utils.h"

namespace jpctracer {
    TEST(cameras, ProjectionCameraTest)
    {
        ProjectionCameraBuilder builder;
        builder.Height = 10;
        builder.Width = 20;
        builder.NearPlane = 1;

        auto camera = Build(builder);
        
        Ray ray = camera({0,0},{0,0});
        Vec3 origin = {0,0,0};
        Vec3 dir = Vec3{-1,0.5,-1}.normalized();

        TestVec3(ray.Origin, origin);
        TestVec3(ray.Direction,dir);


        
    }
}