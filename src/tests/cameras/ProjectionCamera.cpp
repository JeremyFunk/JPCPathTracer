#include "gtest/gtest.h"
#include "jpc_tracer/plugins/cameras/cameras.h"
#include "../test_utils.h"

namespace jpctracer {
    TEST(cameras, ProjectionCameraTest)
    {
        uint Height = 10;
        uint Width = 20;
        uint NearPlane = 1;

        auto camera = camera::ProjectionCamera(NearPlane);
        camera.SetWidthHeight(Width, Height);
        
        Ray ray = camera.Generate({0,0},{0,0});
        Vec3 origin = {0,0,0};
        Vec3 dir = Vec3{-1,0.5,-1}.normalized();

        TestVec3(ray.Origin, origin);
        TestVec3(ray.Direction,dir);


        
    }
}