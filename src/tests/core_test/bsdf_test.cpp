#include "gtest/gtest.h"
#include <memory>
#include "core/Bsdf.h"
#include "core/BsdfGeneric.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"

namespace core {
    
    struct Parameter
    {
        core::Vec3 color = {1,0.1,0.1};
        float pdf;
        core::Vec3 scattered_direction;
    };

    class SingleColorBsdf: public BsdfClosureGeneric<Parameter>
    {
    public:
        virtual BsdfResult EvaluateAll(const Parameter* params, const Vec3& scattered_direction,const Vec2& random_point) const
        {
            return BsdfResult();
        }
        virtual Spectrum Scattering(const Parameter* params, const Vec3& scattered_direction,const Vec3& incident_direction) const
        {
            return Spectrum::FromRGB(params->color);
        }
        virtual Prec Pdf(const Parameter* params, const Vec3& scattered_direction,const Vec3& incident_direction) const
        {
            return params->pdf;
        }
        virtual Vec3 SampleIncidentDirection(const Parameter* params, const Vec3& scattered_direction, const Vec2& random_point) const
        {
            return params->scattered_direction;
        }

        ~SingleColorBsdf() {
        }
    };

    BsdfMemoryPtr CreateBsdf()
    {
        auto bsdfclosure = std::make_shared<SingleColorBsdf>();
        BsdfGeneric<Parameter> bsdf(std::move(bsdfclosure));
        BsdfMemoryInfo info = {(int)bsdf.GetMaxSize()*2,2};
        BsdfMemoryPtr bsdf_memory = CreateBsdfMemory(info);

        Parameter* params1 = bsdf.Setup(bsdf_memory, 0.6);
        Parameter* params2 = bsdf.Setup(bsdf_memory, 0.4);

        params1->pdf = 0.2;
        params1->color = {1,0,0};
        params1->scattered_direction = {1,1,1};

        params2->pdf = 0.3;
        params2->color = {0,0,1};
        params2->scattered_direction = {0,0,0};

        return bsdf_memory;
    }

    TEST(Bsdf,generic_scattering)
    {
        BsdfMemoryPtr bsdf_memory = CreateBsdf();
        SpectrumPasses color = ScatteringBsdf(bsdf_memory,{0,0,0}, {0,0,0});

        Vec3 rgb = color.GetCombined().ToRGB();

        EXPECT_NEAR(rgb[0], 0.6,0.0001);
        EXPECT_NEAR(rgb[1], 0,0.0001);
        EXPECT_NEAR(rgb[2],0.4,0.0001);
    }
}

