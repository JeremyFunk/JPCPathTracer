#include<gtest/gtest.h>
#include <utility>
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/plugins/shaders/bsdf/MixBsdf.h"
#include "jpc_tracer/plugins/shaders/cache/ShaderCache.h"
#include "jpc_tracer/plugins/shaders/bsdf/RootBsdf.h"
#include <iostream>
#include <type_traits>
#include "jpc_tracer/plugins/shaders/bsdf/DebugShader.h"

namespace jpctracer::shader::detail {
    

    TEST(shaders, FlattenNodes1)
    {
        using type = typename FlattenNodes<MaterialType::EMISSION, DebugShader<Spectrum>>::type;
        ////TD<type> test_t;
        static_assert(std::same_as<type,std::tuple<>>);
    }

    TEST(shaders, FlattenNodes2)
    {
        detail::DebugShaderBuilder<Spectrum> builder(Black());
        ShaderCache cache;

        auto mixer = BsdfMixerBuilder(builder,builder,0.2);
        //TD<decltype(mixer)> test_t;

        using mixer_t = MixerNode<DebugShader<Spectrum>,DebugShader<Spectrum>,float>;

        using type = typename FlattenNodes<MaterialType::BSDF, mixer_t>::type;
        static_assert(std::same_as<type,std::tuple<DebugShaderClosure,DebugShaderClosure>>);

    }

    TEST(shaders, BsdfCreator)
    {
        static_assert(! __BsdfCreator<DebugShader<Spectrum>,MaterialType::EMISSION>);
    }

    TEST(shader, SetupMixer)
    {
        DebugShaderClosure m(Black());
        DebugShaderClosure m2 = m;
        ShaderCache cache;
        std::tuple<DebugShaderClosure> tuples{DebugShaderClosure{Black()}};
        std::array<float,1> weights;
        

        SetupMixer<MaterialType::BSDF,0>(DebugShaderBuilder(Black()),tuples,weights,
        SharedMemory());
    }

    TEST(shaders,MixBsdf1)
    {
        auto builder = DefaultDebugShader();
        ShaderCache cache;

        auto test = Build(builder,cache);

        

        CreateDistribution<MaterialType::EMISSION>(test,Ray(),SurfaceInteraction());
        
    }


}
namespace jpctracer {

    TEST(shaders,MixBsdf2)
    {
        auto builder = shader::DefaultDebugShader();
        shader::ShaderCache cache;

        auto mixer_builder = shader::detail::BsdfMixerBuilder(builder,builder,0.2);

        auto mixer = Build(builder,cache);

        auto bsdf = CreateDistribution<MaterialType::BSDF>(mixer,Ray(),SurfaceInteraction());

        static_assert(cts::ShaderBuilder<decltype(mixer_builder)>);
        static_assert(cts::DistributionCreator<decltype(mixer)>);

        static_assert(cts::DistributionFunction<decltype(bsdf)>);


    }

    TEST(shaders,MixBsdf3)
    {
        auto material1 = shader::DefaultDebugShader()
                    .SetColor(FromRGB({1,0,0}));
        auto material2 = shader::DefaultDebugShader()
                    .SetColor(FromRGB({0,1,0}));

        auto material3 = shader::DefaultDebugShader()
                    .SetColor(FromRGB({0,0,1}));

        auto mixer1 = shader::DefaultMaterialMixer()
                .SetFirst(material1)
                .SetSecond(material2)
                .SetInfluence(0.4);

        //shader::detail::TD<decltype(shader::DefaultMaterialMixer())> test_t;

        auto mixer2 = shader::DefaultMaterialMixer()
                    .SetFirst(mixer1)
                    .SetSecond(material3)
                    .SetInfluence(0.2);
        

        shader::ShaderCache cache;

        auto shader = Build(mixer2,cache);

        auto dist = CreateDistribution<MaterialType::BSDF>(shader,Ray(),SurfaceInteraction());

        auto[val,pdf] = dist(Ray());

        EXPECT_NEAR(val[0], 0.2*0.4, 0.01);
        EXPECT_NEAR(val[1], 0.2*0.6, 0.01);

        EXPECT_NEAR(val[2], 0.8, 0.001);
        EXPECT_NEAR(val[3], 1, 0.001);
                


    }

    

}