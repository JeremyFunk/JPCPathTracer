/*#include "gtest/gtest.h"
#include "bsdfs/BSDF.h"
#include "bsdfs/DiffuseBxdf.h"
#include "bsdfs/IBXDF.h"
#include "core/IBSDF.h"
#include "core/Spectrum.h"
#include "samplers/StratifiedSampler.h"

namespace bsdfs {

    using namespace core;
    TEST(bsdfs_test,word_to_local)
    {
        IBXDF* diffuse = new bsdfs::DiffuseBxdf(Spectrum::FromRGB({1,1,1}));
        Vec3 normal = {0.4,0.5,.2};
        normal.normalize();
        BSDF bsdf(diffuse,1,normal);
        //samplers::StratifiedSampler
        delete diffuse;
    }
}*/