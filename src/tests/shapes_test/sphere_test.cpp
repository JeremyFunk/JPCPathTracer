#include "gtest/gtest.h"
#include "core/Linalg.h"
#include "shapes/Sphere.h"
#include "core/Ray.h"
namespace jpc_tracer {

TEST(shapes_test, sphere_single_point_intersection)
{
    Sphere obj(Vec3(1,0,-10), 1, nullptr);

    Ray ray(Vec3(0,0,0), Vec3(0,0,-1));

    auto out = obj.Intersect(ray);

    EXPECT_EQ(out.has_value(), true);

    EXPECT_GT(out->Distance, 0);

    std::cout << out->Distance << ' ' << out->UV.x() << ' ' << out->UV.y() << '\n';
}

TEST(shapes_test, sphere_wrong_dir_single_point_intersection)
{
    Sphere obj(Vec3(1,0,10), 1, nullptr);

    Ray ray(Vec3(0,0,0), Vec3(0,0,-1));

    auto out = obj.Intersect(ray);

    EXPECT_EQ(out.has_value(), false);

    std::cout << out->Distance << ' ' << out->UV.x() << ' ' << out->UV.y() << '\n';
}

TEST(shapes_test, sphere_two_point_intersection)
{
    Sphere obj(Vec3(0,0,-10), 1, nullptr);

    Ray ray(Vec3(0,0,0), Vec3(0,0,-1));

    auto out = obj.Intersect(ray);

    EXPECT_EQ(out.has_value(), true);

    EXPECT_GT(out->Distance, 0);

    EXPECT_EQ(out->Distance, 9);

    std::cout << out->Distance << ' ' << out->UV.x() << ' ' << out->UV.y() << '\n';
}

TEST(shapes_test, sphere_wrong_dir_two_point_intersection)
{
    Sphere obj(Vec3(0,0,10), 1, nullptr);

    Ray ray(Vec3(0,0,0), Vec3(0,0,-1));

    auto out = obj.Intersect(ray);

    EXPECT_EQ(out.has_value(), false);

    std::cout << out->Distance << ' ' << out->UV.x() << ' ' << out->UV.y() << '\n';
}

TEST(shapes_test, sphere_no_intersection)
{
    Sphere obj(Vec3(10,0,-10), 1, nullptr);

    Ray ray(Vec3(0,0,0), Vec3(0,0,-1));

    auto out = obj.Intersect(ray);

    EXPECT_EQ(out.has_value(), false);

    std::cout << out->Distance << ' ' << out->UV.x() << ' ' << out->UV.y() << '\n';
}
}