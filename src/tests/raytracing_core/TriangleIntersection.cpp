#include "gtest/gtest.h"
#include "jpc_tracer/engine/raytracing/SurfaceInteraction.h"
#include "jpc_tracer/engine/raytracing/detail/intersection/acceleration/TriangleIntersection.h"

namespace jpctracer::raytracing {
    TEST(triangle_intersection, InSurface)
    {
        Vec3 v1 = {0.75,-0.5,0};
        Vec3 v2 = {1,1,1};
        Vec3 v3 = {-0.75,-0.25,2};

        Vec3 s_1 = v2-v1;
        Vec3 s_2 = v3-v1;

        Vec3 normal = {0.6687424182891846, -0.4863581359386444, 0.5623515844345093};
        
        Vec2 uv_1 = {1,0};
        Vec2 uv_2 = {1,0.805};
        Vec2 uv_3 = {0,0.486};

        Vec2 expected_uv = {0.674,0.282};
        Vec3 expected_position = {0.2993,-0.18873,0.80517};

        Vec3 start_origin = {1.1472,-1.2257,1.01};

        Ray ray {expected_position-start_origin,start_origin};

        Prec expected_distance = ray.Direction.norm();
        ray.Direction = ray.Direction.normalized();

        auto intersection = TriangleIntersect(ray, v1, s_1, s_2);
        EXPECT_TRUE(intersection.has_value());

        SurfaceInteraction interaction = TriangleGetInteraction(ray, intersection.value(), 0, normal, normal, normal, uv_1, uv_2, uv_3);

        EXPECT_NEAR((interaction.Normal-normal).norm(), 0, 0.001);
        EXPECT_NEAR((interaction.Point-expected_position).norm(),0,0.001);
        EXPECT_NEAR(interaction.Distance,expected_distance,0.001);
        EXPECT_NEAR((interaction.UV-expected_uv).norm(),0,0.001);   
    }

    TEST(triangle_intersection, OutSurface1)
    {
        Vec3 v1 = {0.75,-0.5,0};
        Vec3 v2 = {1,1,1};
        Vec3 v3 = {-0.75,-0.25,2};

        Vec3 s_1 = v2-v1;
        Vec3 s_2 = v3-v1;

        Vec3 start_origin = {1.1472,-1.2257,1.01};
        Vec3 ray_dir_point = {0,0.75,1.75};

        Ray ray {ray_dir_point-start_origin,start_origin};

        Prec expected_distance = ray.Direction.norm();
        ray.Direction = ray.Direction.normalized();

        auto intersection = TriangleIntersect(ray, v1, s_1, s_2);
        EXPECT_FALSE(intersection.has_value());
  
    }

    TEST(triangle_intersection, OutSurface2)
    {
        Vec3 v1 = {0.75,-0.5,0};
        Vec3 v2 = {1,1,1};
        Vec3 v3 = {-0.75,-0.25,2};

        Vec3 s_1 = v2-v1;
        Vec3 s_2 = v3-v1;

        Vec3 start_origin = {1.1472,-1.2257,1.01};
        Vec3 ray_dir_point = {2.5,-3,1};

        Ray ray {ray_dir_point-start_origin,start_origin};

        Prec expected_distance = ray.Direction.norm();
        ray.Direction = ray.Direction.normalized();

        auto intersection = TriangleIntersect(ray, v1, s_1, s_2);
        EXPECT_FALSE(intersection.has_value());
  
    }

    TEST(triangle_intersection, OutSurface3)
    {
        Vec3 v1 = {0.75,-0.5,0};
        Vec3 v2 = {1,1,1};
        Vec3 v3 = {-0.75,-0.25,2};

        Vec3 s_1 = v2-v1;
        Vec3 s_2 = v3-v1;

        Vec3 start_origin = {1.1472,-1.2257,1.01};
        Vec3 ray_dir_point = {-1,0.2,0.75};

        Ray ray {ray_dir_point-start_origin,start_origin};

        Prec expected_distance = ray.Direction.norm();
        ray.Direction = ray.Direction.normalized();

        auto intersection = TriangleIntersect(ray, v1, s_1, s_2);
        EXPECT_FALSE(intersection.has_value());
  
    }

    TEST(triangle_intersection, OutSurface4)
    {
        Vec3 v1 = {0.75,-0.5,0};
        Vec3 v2 = {1,1,1};
        Vec3 v3 = {-0.75,-0.25,2};

        Vec3 s_1 = v2-v1;
        Vec3 s_2 = v3-v1;

        Vec3 start_origin = {1.1472,-1.2257,1.01};
        Vec3 ray_dir_point = {0.9,1,0.5};

        Ray ray {ray_dir_point-start_origin,start_origin};

        Prec expected_distance = ray.Direction.norm();
        ray.Direction = ray.Direction.normalized();

        auto intersection = TriangleIntersect(ray, v1, s_1, s_2);
        EXPECT_FALSE(intersection.has_value());
  
    }
}