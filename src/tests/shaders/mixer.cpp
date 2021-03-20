#include "gtest/gtest.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/plugins/shaders/cache/ShaderCache.h"
#include "jpc_tracer/plugins/shaders/values/Mixer.h"

namespace jpctracer {
    TEST(shaders,MixColor)
    {
        ColorBuilder auto c = Spectrum{};
        auto mixer_builder = DefaultColorMixer()
                        .SetFrist(c)
                        .SetSecond(2.0);
        plugins::ShaderCache cache;
        auto mixer = plugins::Build(mixer_builder,cache);
        Eval(mixer,SurfaceInteraction{});

    }
}