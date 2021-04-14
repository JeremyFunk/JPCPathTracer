
#include "gtest/gtest.h"
#include <bits/c++config.h>
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include "../test_utils.h"

namespace jpctracer {
    
    TEST(shaders,factor_debugbsdf)
    {
        auto f = [&](ShaderContext* ctx)
        {
            BsdfNode node1 = DebugBsdf(ctx, FromRGB({0,0,1}));
            BsdfNode node2 = DebugBsdf(ctx, FromRGB({1,0,0}));
            return MixBsdf(ctx, node1, node2, 0.5);
        };
        Ray rays[2] = {Ray{{0,0,0},{0,0,0}},Ray{{0,0,0},{0,0,0}}};
        ShaderResults results = shadersys::EvalShader<MATERIAL_DIFFUSE>(f, {0,1,1},{rays,2});
        TestSpectrum(results.eval_results[0].luminance,FromRGB({0.5,0,0.5}));

    }
    
    TEST(shaders,factor_debugbsdf2)
    {
        auto f = [&](ShaderContext* ctx)
        {
            BsdfNode node1 = DebugBsdf(ctx, FromRGB({0,0,1}));
            BsdfNode node2 = DebugBsdf(ctx, FromRGB({1,0,0}));
            BsdfNode node3 = DebugBsdf(ctx, FromRGB({0,1,0}));
            BsdfNode mix1 = MixBsdf(ctx, node1, node2, 0.5);
            return MixBsdf(ctx,node3,mix1,0.5);
        };
        Ray rays[2] = {Ray{{0,0,0},{0,0,0}},Ray{{0,0,0},{0,0,0}}};
        ShaderResults results = shadersys::EvalShader<MATERIAL_DIFFUSE>(f, {0,1,1} ,{rays,2});
        TestSpectrum(results.eval_results[0].luminance,FromRGB({0.25,0.5,0.25}));
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

}