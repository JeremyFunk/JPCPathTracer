#include "gtest/gtest.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/engine/shadersystem/NormalSpace.h"
#include "jpc_tracer/plugins/shaders/GlossyBsdf.h"

namespace jpctracer {
    TEST(shaders,GlossyBsdfTest1)
    {
        Vec3 normal = {0,1,0};
        Prec alpha = 0.2;
        Vec3 scattered_dir = {-1,1,-1};
        scattered_dir *= -1;
        Vec3 incident_dir = {1,1,1};
        Spectrum color = FromRGB({1,1,1});

        scattered_dir = scattered_dir.normalized();
        incident_dir = incident_dir.normalized();

        Ray scattered_ray = {scattered_dir,{0,0,0}};
        Ray incident_ray = {incident_dir,{0,0,0}};
        

        SurfaceInteraction interaction = {{0,0,0},normal};

        shadersys::NormalSpace n_space = shadersys::CreateNormalSpace(scattered_ray, interaction);


        incident_ray = shadersys::WorldToNormal(incident_ray,n_space);

        GlossyBsdfClosure closure{color,0, n_space.ScatteringRay};

        closure.m_alpha = alpha;

        ShaderResult result = closure.Eval(incident_ray);

        Prec cos = Vec3{1,1,1}.normalized().dot(normal);

        Vec3 rgb = ToRGB(result.luminance);
        Prec pdf = result.pdf;
        Prec test_val = ( rgb[0]/pdf) *cos;

        EXPECT_NEAR(rgb[0],6,0.1);
        EXPECT_NEAR(rgb[1],6,0.1);
        EXPECT_NEAR(rgb[2],6,0.1);

        EXPECT_NEAR(test_val,1,0.1);
        EXPECT_NEAR(pdf, 3.45,0.01);
    }
}