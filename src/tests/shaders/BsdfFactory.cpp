
#include "../test_utils.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/shadersystem/ShaderResults.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include "gtest/gtest.h"

namespace jpctracer
{

TEST(shaders, factor_debugbsdf)
{
    auto f = [](Ray scattered) {
        BsdfNode node1 = DebugBsdf(srgb::FromRGB({0, 0, 1}));
        BsdfNode node2 = DebugBsdf(srgb::FromRGB({1, 0, 0}));
        return Mix(node1, node2, 0.5);
    };
    Ray rays[2] = {Ray{}, Ray{}};

    shadersys::ShaderResultsStack stack;
    auto result = stack.CreateCombined(2, 0);
    shadersys::EvalShader(f, Ray{}, {rays, 2}, result);
    TestSpectrum(result.eval_bsdf[0].value, srgb::FromRGB({0.5, 0, 0.5}));
}

TEST(shaders, factor_debugbsdf2)
{
    Logger::Init();
    auto f = [](Ray scattered) {
        BsdfNode node1 = DebugBsdf(srgb::FromRGB({0, 0, 1}));
        BsdfNode node2 = DebugBsdf(srgb::FromRGB({1, 0, 0}));
        BsdfNode node3 = DebugBsdf(srgb::FromRGB({0, 1, 0}));
        BsdfNode mix1 = Mix(node1, node2, 0.5);
        return Mix(node3, mix1, 0.5);
    };
    shadersys::ShaderResultsStack stack;
    Ray rays[2] = {Ray{}, Ray{}};
    auto result = stack.CreateCombined(2, 0);
    shadersys::EvalShader(f, Ray{}, {rays, 2}, result);
    TestSpectrum(result.eval_bsdf[0].value, srgb::FromRGB({0.25, 0.5, 0.25}));
}
/*
TEST(shaders,dist_factory)
{
    DistributionFactory* factory = new DistributionFactory();
    auto shader_func = [](ShaderContext context)
    {
        BsdfNode* bsdf1 = DebugBsdf(context, FromRGB({0,0,1}));
        BsdfNode* bsdf2 = DebugBsdf(context, FromRGB({1,0,0}));
        return MixBsdf(context,bsdf1,bsdf2,0.2);
    };
    {
        RootShader shader = factory->Create<MATERIAL_BSDF>(shader_func,Ray(),SurfaceInteraction());
        auto[val,pdf] = shader(Ray());
        TestSpecturm(val, FromRGB({0.8,0,0.2}));
    }


}

TEST(shaders,dist_factory2)
{
    BsdfStack* pool = new BsdfStack();
    BsdfMemoryState state_begin = pool->BsdfsState();
    auto shader_func = [](ShaderContext context)
    {
        BsdfNode* bsdf1 = DebugBsdf(context, FromRGB({0,0,1}));
        BsdfNode* bsdf2 = DebugBsdf(context, FromRGB({1,0,0}));
        return MixBsdf(context,bsdf1,bsdf2,0.2);
    };

    {
        RootShader shader = CreateShader<MATERIAL_BSDF>(shader_func,pool,NormalSpace());
        auto[val,pdf] = shader(Ray());
        TestSpecturm(val, FromRGB({0.8,0,0.2}));
    }
    BsdfMemoryState state = pool->BsdfsState();
    EXPECT_EQ(state.count,state_begin.count);
    EXPECT_EQ(state.free_space_size,state_begin.free_space_size);
    EXPECT_EQ(state.next,state_begin.next);


}

TEST(shaders,dist_factory3)
{
    DistributionFactory* factory = new DistributionFactory();
    auto bsdf1 = ShaderBind(DebugBsdf,FromRGB({0,0,1}));
    //FromRGB({1,0,0}
    auto bsdf2 = ShaderBind(DebugBsdf,FromRGB({1,0,0}));

    auto shader_func = ShaderBind(MixBsdf,bsdf1,bsdf2,0.2);
    {
        RootShader shader = factory->Create<MATERIAL_BSDF>(shader_func,Ray(),SurfaceInteraction());
        auto[val,pdf] = shader(Ray());
        TestSpecturm(val, FromRGB({0.8,0,0.2}));
    }


}*/

} // namespace jpctracer