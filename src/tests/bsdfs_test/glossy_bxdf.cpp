/*#include "BxdfDebugger.h"
#include "bsdfs/TorranceSparrowBRDF.h"
#include "bsdfs/BeckmannDistribution.h"
#include "bsdfs/IFresnel.h"
#include "core/Spectrum.h"
#include "gtest/gtest.h"
namespace bsdfs {

    using namespace core;
    TEST(bsdfs_test,GlossyBxdf)
    {
        FresnelNoOp fresnel;
        BeckmannDistribution dist(1);
        auto base_color = Spectrum::FromRGB({0.5,0.5,0.5});
        TorranceSparrowBRDF bxdf(base_color,&fresnel,&dist);
        core::Vec3 in_vec;
        in_vec[0] = -0.3f;
        in_vec[1] = -0.4f;
        in_vec[2] = std::sqrt(1-in_vec[0]* in_vec[0]- in_vec[1]* in_vec[1]);
        BxdfDebugSampleUniform(&bxdf, "beckmannbxdf", in_vec);
    }

    
    TEST(bsdfs_test,Beckmann)
    {
        FresnelNoOp fresnel;
        BeckmannDistribution dist(0.3);
        core::Vec3 in_vec;
        in_vec[0] = -0.7f;
        in_vec[1] = -0.4f;
        in_vec[2] = std::sqrt(1-in_vec[0]* in_vec[0]- in_vec[1]* in_vec[1]);
        DistDebugSampleUniform(&dist, "Beckmann", in_vec);
    }
}*/