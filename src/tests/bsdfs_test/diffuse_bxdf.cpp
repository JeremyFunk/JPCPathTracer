/*#include "BxdfDebugger.h"
#include "bsdfs/DiffuseBxdf.h"
#include "core/Spectrum.h"
#include "gtest/gtest.h"
namespace jpc_tracer {


    TEST(bsdfs_test,DiffuseBxdf)
    {
        DiffuseBxdf bxdf(Spectrum::FromRGB({0.5,0.5,0.5}));
        Vec3 in_vec;
        in_vec[0] = 0.2f;
        in_vec[1] = 0.3f;
        in_vec[2] = std::sqrt(1-in_vec[0]* in_vec[0]- in_vec[1]* in_vec[1]);
        BxdfDebugSampleUniform(&bxdf, "diffusebxdf", in_vec);

    }
}*/