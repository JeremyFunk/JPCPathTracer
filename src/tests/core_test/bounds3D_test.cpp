#include "gtest/gtest.h"
#include <optional>
#include "core/Bounds3D.h"
#include "core/Linalg.h"
#include "core/Ray.h"

namespace jpc_tracer
{
    // TEST(core, bounds3d_intersection)
    // {
    //     Bounds3D<Prec> bound(1,-1,1,-1,-3,-4);

    //     Ray ray({0,0,0}, {0,0,-1});

    //     auto min = std::make_optional<Prec>(1000);

    //     int dir[3] = {0,0,0};

    //     auto out = bound.IsIntersecting(ray, min, {0,0,0}, dir);

    //     EXPECT_TRUE(out);
    // }

    void compare_bounds3d(const Bounds3D<Prec>& bound1, const Vec3& max, const Vec3& min)
    {
        for (int i= 0; i < 3; i++)
        {
            EXPECT_EQ(bound1.Max[i], max[i]);
            EXPECT_EQ(bound1.Min[i], min[i]);
        }
    }

    TEST(core, bounds3d_union)
    {
        Bounds3D<Prec> bound1(6,5,4,3,2,1);
        Bounds3D<Prec> bound2(7,3,1,1,0,-2);

        auto out = Union(bound1, bound2);

        Vec3 compMax {7,4,2};
        Vec3 compMin {3,1,-2};

        compare_bounds3d(out, compMax, compMin);

        out = Union(bound1, compMax);

        Vec3 compVecMax {7,4,2};
        Vec3 compVecMin {5,3,1};

        compare_bounds3d(out, compVecMax, compVecMin);
    }

    TEST(core, bounds3d_union_intern)
    {
        Bounds3D<Prec> bound1(6, 5, 4, 3, 2, 1);
        Bounds3D<Prec> bound2(7, 3, 1, 1, 0, -2);

        bound1.Union(bound2);

        Vec3 compMax{ 7,4,2 };
        Vec3 compMin{ 3,1,-2 };

        compare_bounds3d(bound1, compMax, compMin);
    }
}
