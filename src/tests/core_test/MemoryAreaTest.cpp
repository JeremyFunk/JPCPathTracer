#include "gtest/gtest.h"
#include "core/IBSDF.h"
#include "bsdfs/BSDF.h"
#include "core/Linalg.h"
#include "core/MemoryArea.h"


namespace core {
    /*
    struct Interface
    {
        virtual Vec3 GetNormale() = 0;
    };
    struct Testcls : public Interface
    {
        Testcls(const Vec3& normal) : Normal(normal) {}
        Vec3 Normal;
        virtual Vec3 GetNormale() override
        {
            return Normal;
        }
    };

    TEST(core_test,mem_area_eigen)
    {
        MemoryArea memory_area;
        Vec3 normal = {1,0,0};
        Testcls* cls = memory_area.Create<Testcls>(normal);
        Interface* casted = cls;
        EXPECT_TRUE(normal[0]== casted->GetNormale()[0] &&
        normal[1]== casted->GetNormale()[1] &&
        normal[2]== casted->GetNormale()[2]);


    }

    TEST(core_test,mem_area_bsdfs2)
    {
        MemoryArea memory_area;
        Vec3 normal = {1,0,0};
        bsdfs::IBXDF* diffuse = (bsdfs::IBXDF*) new bsdfs::DiffuseBxdf(core::Spectrum::FromValue(1));
        bsdfs::BSDF* bsdf = memory_area.Create<bsdfs::BSDF>(diffuse,1,normal);
        bsdf->Evaluate(normal, { 0.1,0.2 });
    }

    TEST(core_test,mem_area_bsdfs)
    {
        MemoryArea memory_area;

        bsdfs::IBXDF* bxdf = (bsdfs::IBXDF*) memory_area.Create<bsdfs::DiffuseBxdf>(core::Spectrum::FromRGB({ 0,1,1 }));
        std::vector<bsdfs::IBXDF*>bxdfs = { bxdf };
        Vec3 normal = {1,0,0};
        //bsdfs::BSDF* bsdf = memory_area.Create<bsdfs::BSDF>(bxdfs,normal);

    }*/
}