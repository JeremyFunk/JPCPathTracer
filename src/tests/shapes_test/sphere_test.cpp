#include "gtest/gtest.h"
#include "core/Linalg.h"
#include "shapes/Sphere.h"
#include "core/Ray.h"

TEST(shapes_test, sphere)
{
    shapes::Sphere obj(core::Vec3(2,0,0), 1, nullptr);

    core::Ray ray(core::Vec3(0,0,0), core::Vec3(1,0,0));

    auto out = obj.Intersect(ray);

    EXPECT_EQ(out.has_value(), true);

    std::cout << "vlt";
}