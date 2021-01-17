#include "gtest/gtest.h"
#include <optional>
#include "core/Bounds3D.h"
#include "core/Linalg.h"
#include "core/Ray.h"

namespace core_test
{
    // TEST(core, bounds3d_intersection)
    // {
    //     core::Bounds3D<core::Prec> bound(1,-1,1,-1,-3,-4);

    //     core::Ray ray({0,0,0}, {0,0,-1});

    //     auto min = std::make_optional<core::Prec>(1000);

    //     int dir[3] = {0,0,0};

    //     auto out = bound.IsIntersecting(ray, min, {0,0,0}, dir);

    //     EXPECT_TRUE(out);
    // }

    TEST(core, bounds3d_union)
    {
        core::Bounds3D<core::Prec> bound1(6,5,4,3,2,1);
        core::Bounds3D<core::Prec> bound2(7,3,1,1,0,-2);

        auto out = core::Union(bound1, bound2);

        core::Vec3 compMax {7,4,2};
        core::Vec3 compMin {3,1,-2};

        EXPECT_EQ(out.Max, compMax);
        EXPECT_EQ(out.Min, compMin);

        out = core::Union(bound1, compMax);

        core::Vec3 compVecMax {7,4,2};
        core::Vec3 compVecMin {5,3,1};

        EXPECT_EQ(out.Max, compVecMax);
        EXPECT_EQ(out.Min, compVecMin);
    }

}
