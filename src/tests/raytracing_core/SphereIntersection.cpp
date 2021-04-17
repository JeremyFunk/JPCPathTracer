#include "gtest/gtest.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/SphereMesh.h"

namespace jpctracer::raytracing {

    auto TestSphere(Vec3 ray_pos,Vec3 ray_dir,Vec3 center, Prec radius)
    {
        Ray ray = {ray_dir,ray_pos};
        SphereMesh mesh = {{{center,radius}},{0}};
        int mat_slot = 0;
        return Intersect(mesh,0,ray,&mat_slot);
    }

    TEST(raytracing,sphere_intersection)
    {
        Ray ray_dir = {{1,1,1},{0,0,0}};
        Prec radius = 1;
        Vec3 pos = {1,2,3};
        
        

    }

    TEST(raytracing, sphere_single_point_intersection)
    {

        auto out = TestSphere({0,0,0},{0,0,-1},{1,0,-10},1);

        EXPECT_EQ(out.has_value(), true);

        EXPECT_GT(out->Distance, 0);

        std::cout << out->Distance << ' ' << out->UV[0] << ' ' << out->UV[1] << '\n';
    }

    TEST(raytracing, sphere_wrong_dir_single_point_intersection)
    {

        auto out = TestSphere({0,0,0},{0,0,-1},{1,0,10},1);

        EXPECT_EQ(out.has_value(), false);

        std::cout << out->Distance << ' ' << out->UV[0] << ' ' << out->UV[1] << '\n';
    }

    TEST(raytracing, sphere_two_point_intersection)
    {
        auto out = TestSphere({0,0,0},{0,0,-1},{0,0,-10},1);

        EXPECT_EQ(out.has_value(), true);

        EXPECT_GT(out->Distance, 0);

        EXPECT_EQ(out->Distance, 9);

        std::cout << out->Distance << ' ' << out->UV[0] << ' ' << out->UV[1] << '\n';
    }

    TEST(raytracing, sphere_wrong_dir_two_point_intersection)
    {

        auto out = TestSphere({0,0,0},{0,0,-1},{0,0,10},1);

        EXPECT_EQ(out.has_value(), false);

        std::cout << out->Distance << ' ' << out->UV[0] << ' ' << out->UV[1] << '\n';
    }

    TEST(raytracing, sphere_no_intersection)
    {
        auto out = TestSphere({0,0,0},{0,0,-1},{10,0,-10},1);

        EXPECT_EQ(out.has_value(), false);

        std::cout << out->Distance << ' ' << out->UV[0] << ' ' << out->UV[1] << '\n';
    }
}