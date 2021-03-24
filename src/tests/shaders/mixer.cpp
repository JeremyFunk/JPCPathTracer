#include "gtest/gtest.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/plugins/shaders/cache/ShaderCache.h"
#include "jpc_tracer/plugins/shaders/values/Mixer.h"

namespace jpctracer {
    TEST(shaders,MixColor)
    {
        shader::ColorBuilder auto c = Black();
        shader::ColorBuilder auto b = FromRGB({1,2,3});
        auto mixer_builder = shader::DefaultColorMixer()
                        .SetFirst(c)
                        .SetSecond(b);
        shader::ShaderCache cache;
        
        auto mixer = shader::detail::Build(mixer_builder,cache);
        Eval(mixer,SurfaceInteraction{});

    }
}