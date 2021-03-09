#include "gtest/gtest.h"
#include "jpc_tracer/maths/maths.h"
#include "jpc_tracer/shaders/Cache.h"
#include "jpc_tracer/shaders/Concepts.h"
#include "jpc_tracer/shaders/values/Mixer.h"

namespace jpctracer {
    TEST(shaders,MixColor)
    {
        ColorBuilder auto c = Spectrum{};
        auto mixer_builder = DefaultColorMixer()
                        .SetFrist(c)
                        .SetSecond(2.0);
        shader::Cache cache;
        auto mixer = shader::Build(mixer_builder,cache);
        Eval(mixer,SurfaceInteraction{});

    }
}