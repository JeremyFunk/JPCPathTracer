#include "gtest/gtest.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/maths.h"
#include "jpc_tracer/plugins/shaders/bsdf/MirrorBsdf.h"
#include "jpc_tracer/plugins/shaders/bsdf/OptimizedBsdf.h"
#include "jpc_tracer/plugins/shaders/cache/ShaderCache.h"
#include "jpc_tracer/plugins/shaders/shaders.h"

template<class T>
class TD;

namespace jpctracer {
    TEST(shaders,MirrorBsdf)
    {
        plugins::ShaderCache cache{};
        auto mirror = Build(shader::MirrorBsdfBuilder(),cache);
        auto dist = mirror.CreateDistribution<MaterialType::BSDF>(Ray(), 
                            SurfaceInteraction());

        
    }
}