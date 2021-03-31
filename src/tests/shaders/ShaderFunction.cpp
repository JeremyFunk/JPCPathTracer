#include <gtest/gtest.h>
#include <utility>
#include "jpc_tracer/plugins/shaders/ShaderContext.h"
#include "jpc_tracer/plugins/shaders/ShaderFunction.h"

namespace jpctracer {
    TEST(shader,ShaderFunc1)
    {
        auto f = [&](jpctracer::ShaderContext context)
        {
            return 2.f;
        };
        ShaderContext context{};
        float t = Eval(context,f);
    }

    float mix(float a,float b, float v)
    {
        return a*v+b*(1-v);
    }
    

    struct TextureBuilder
    {
        float value;
        auto Build()
        {
            return [&](ShaderContext context)
            {
                return value;
            };
        }
    };



    class MaterialSettings
    {
    public:
        void SetFilepath(std::string property,std::string filepaht);
        std::string GetFilepath(std::string property);
        
    };

    auto Build(MaterialSettings settings)
    {
        TextureBuilder text_builder1{1};
        auto tex1 = text_builder1.Build();
        TextureBuilder text_builder2{2};
        auto tex2 = text_builder2.Build();
        

        auto mixed_color = ShaderBind(mix,tex1,tex2,0.5);
        auto result = ShaderBind(mix,mixed_color,0,0);
        return result;
    }

    auto mat1(std::string tex1_path,std::string tex2_path,float mix_value)
    {
        
    }

    TEST(shader,ShaderFunc2)
    {
        MaterialSettings settings;
        TextureBuilder text_builder1{2};
        auto tex1 = text_builder1.Build();
        TextureBuilder text_builder2{3};
        auto tex2 = text_builder2.Build();


        auto mixed_color = ShaderBind(mix,tex1,tex2,0.5);
        auto result = ShaderBind(mix,mixed_color,0,1);
        
        ShaderContext context{};
        EXPECT_NEAR(result(context),2.5,0.001);
    }
}