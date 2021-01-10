#include "core/Linalg.h"
#include "gtest/gtest.h"
#include "bsdfs/SphericalCoordinates.h"
#include "bsdfs/BeckmannDistribution.h"
#include "core/constants.h"

namespace bsdfs {

    core::Prec ComputeDist(core::Prec theta,core::Prec alpha = 0.2)
    {
        core::Vec3 microfacet_normal = bsdfs::SphericalDirection(theta/ 360.f *  2* M_PI,0);
        
        bsdfs::BeckmannDistribution dist(alpha );
        return dist.Distribution(microfacet_normal);
    }

    core::Prec ShadowDist(core::Prec theta,core::Prec alpha = 0.2)
    {
        core::Vec3 microfacet_normal = bsdfs::SphericalDirection(theta/ 360.f *  2* M_PI,0);
        
        bsdfs::BeckmannDistribution dist(alpha );
        return dist.Distribution(microfacet_normal);
    }

    TEST(beckmann, dist_val1)
    {
        core::Prec val = ComputeDist(0);

        EXPECT_NEAR(val,8.0f,0.1);
    }

    TEST(beckmann, dist_val2)
    {
        core::Prec val = ComputeDist(15);

        EXPECT_NEAR(val,1.5f,0.1);
    }

    TEST(beckmann, shadow_val1)
    {
        core::Prec val = ComputeDist(0);

        EXPECT_NEAR(val,8.0f,0.1);
    }
}