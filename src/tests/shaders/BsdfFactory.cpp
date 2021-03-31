
#include "gtest/gtest.h"
#include <bits/c++config.h>
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/plugins/shaders/bsdf/BsdfStack.h"
#include "jpc_tracer/plugins/shaders/bsdf/DebugBsdf.h"
#include "jpc_tracer/plugins/shaders/ShaderContext.h"
#include "jpc_tracer/plugins/shaders/bsdf/RootShader.h"
#include "jpc_tracer/plugins/shaders/cache/NormalSpace.h"
#include "jpc_tracer/plugins/shaders/DistributionFactory.h"
#include "jpc_tracer/plugins/shaders/shaders.h"
#include "../test_utils.h"

namespace jpctracer {
    
    TEST(shaders,factor_debugbsdf)
    {
        constexpr std::size_t bsdf_count = 20;
        NormalSpace normal_space;
        BsdfStack* factory = new BsdfStack();
        ShaderContext context{&normal_space,MATERIAL_DIFFUSE,factory};
        for(int i=0; i<13;i++)
        {
            BsdfNode* bsdf = CreateBsdf<MATERIAL_DIFFUSE, DebugBsdfClosure>(context,Black());
            
            EXPECT_TRUE(bsdf!=nullptr);

        }
        free(factory);
    }

    TEST(shaders,factor_debugbsdf2)
    {
        constexpr std::size_t bsdf_count = 20;
        NormalSpace normal_space;
        
        BsdfStack* factory = new BsdfStack();
        ShaderContext context{&normal_space,MATERIAL_DIFFUSE,factory};
        BsdfNode* bsdf = CreateBsdf<MATERIAL_EMISSION, DebugBsdfClosure>(context,Black());
        EXPECT_EQ(bsdf,nullptr);
        free(factory);
    }

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
        

    }

}