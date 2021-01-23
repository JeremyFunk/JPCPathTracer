#include "gtest/gtest.h"
#include <memory>
#include "bsdfs/CookTorranceBSDF.h"
#include "bsdfs/BeckmannDistribution.h"
#include "bsdfs/IFresnel.h"
#include "core/Bsdf.h"
#include "core/BsdfGeneric.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
namespace jpc_tracer {
    TEST(glossy,roughness0)
    {
        auto fresnel = std::make_shared<FresnelNoOp>();
        auto distribution = std::make_shared<BeckmannDistribution>();
        using BsdfParamsT = CookTorranceParams<int, BeckmannParams>;
        using BsdfClosureT = CookTorranceBSDFClosure<int, BeckmannParams>;
        
        auto bsdf_closure =std::make_shared<BsdfClosureT>(fresnel,distribution);
        auto bsdf = std::make_shared<BsdfGeneric<BsdfParamsT>>(bsdf_closure);

        BsdfMemoryInfo info;
        info.max_byte_size = bsdf->GetMaxSize();
        info.max_bsdf_count = 1;

        auto memory = CreateBsdfMemory(info);
        ResetBsdfMemory(memory, {0,1,0});

        auto params = bsdf->Setup(memory,1);
        params->Distribution.Alpha = 0.023;
        params->Reflectance = Spectrum::FromRGB({1,1,1});

        Vec3 scattered_direction = {-1,1,-1};
        scattered_direction = scattered_direction.normalized();
        Vec3 incident_direction = {1,1,1};
        incident_direction = incident_direction.normalized();

        SpectrumPasses color =  ScatteringBsdf(memory, scattered_direction, incident_direction);

        Vec3 rgb = color.GetCombined().ToRGB();

        Prec pdf = PdfBsdf(memory, scattered_direction, incident_direction);

        //EXPECT_EQ(rgb[0],1);
        //EXPECT_EQ(rgb[1],1);
        //EXPECT_EQ(rgb[2],1);
        EXPECT_NEAR(pdf,0,0.0001);
        
    }

    TEST(glossy,roughness02)
    {
        auto fresnel = std::make_shared<FresnelNoOp>();
        auto distribution = std::make_shared<BeckmannDistribution>();
        using BsdfParamsT = CookTorranceParams<int, BeckmannParams>;
        using BsdfClosureT = CookTorranceBSDFClosure<int, BeckmannParams>;
        
        auto bsdf_closure =std::make_shared<BsdfClosureT>(fresnel,distribution);
        auto bsdf = std::make_shared<BsdfGeneric<BsdfParamsT>>(bsdf_closure);

        BsdfMemoryInfo info;
        info.max_byte_size = bsdf->GetMaxSize();
        info.max_bsdf_count = 1;

        auto memory = CreateBsdfMemory(info);
        Vec3 normal = {0,1,0};
        ResetBsdfMemory(memory, normal);

        auto params = bsdf->Setup(memory,1);
        params->Distribution.Alpha = 0.2;
        params->Reflectance = Spectrum::FromRGB({1,1,1});

        Vec3 scattered_direction = {-1,1,-1};
        scattered_direction = scattered_direction.normalized();
        Vec3 incident_direction = {1,1,1};
        incident_direction = incident_direction.normalized();

        SpectrumPasses color =  ScatteringBsdf(memory, scattered_direction, incident_direction);

        Prec pdf = PdfBsdf(memory, scattered_direction, incident_direction);

        Vec3 rgb = color.GetCombined().ToRGB();

        EXPECT_NEAR(rgb[0],6,0.1);
        EXPECT_NEAR(rgb[1],6,0.1);
        EXPECT_NEAR(rgb[2],6,0.1);

        Prec cos = incident_direction.dot(normal);

        Prec test_val = ( rgb[0]/pdf) *cos;

        EXPECT_NEAR(test_val,1,0.1);
        EXPECT_NEAR(pdf,0.2,0.0001);
    }
}