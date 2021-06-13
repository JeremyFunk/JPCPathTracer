#include "ParseScene.h"
#include "app/Archive.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include "jpc_tracer/plugins/shaders/GlossyBsdf.h"
#include "jpc_tracer/plugins/shaders/LambertianBsdf.h"

namespace jpctracer::parse
{

struct GlossyDiff
{
    Spectrum color = FromRGB({0.5, 0.5, 0.5});
    Prec roughness = 0.0;
    Prec mix = 0.5;

    auto bsdf()
    {
        return [=](Ray scattered) {
            auto b1 = DiffuseBsdf(scattered, color);
            auto b2 = GlossyBsdf(scattered, FromRGB({1, 1, 1}), roughness);
            return Mix(b1, b2, mix);
        };
    }
};

struct DiffuseMaterial{
    Spectrum color = FromRGB({0.5,0.5,0.5});
    auto bsdf()
    {
        return [=](Ray scattered){
            return DiffuseBsdf(scattered,color);
        };
    }
};

struct DebugMaterial
{
    Spectrum color = FromRGB({0.1, 0.2, 0.6});
    auto bsdf()
    {
        return [=](Ray scattered) -> BsdfNode { return DebugBsdf(color); };
    }
};

// clang-format off
bool LoadGlossyDiff(ShaderArchive& ar, GlossyDiff& sh)
{
    return Load(ar,"color",sh.color)
        || Load(ar,"roughness",sh.roughness)
        || Load(ar,"mix",sh.mix);
}
bool LoadDebugMaterial(ShaderArchive& ar, DebugMaterial& mat)
{
    return Load(ar,"color",mat.color);
}
bool LoadDiffuseMaterial(ShaderArchive& ar, DiffuseMaterial& mat)
{
    return Load(ar,"color",mat.color);
}
bool LoadMaterial(ShaderArchive& ar)
{
    return LoadShader(ar, "GlossyDiff", LoadGlossyDiff)
        || LoadShader(ar,"DebugShader",LoadDebugMaterial)
        || LoadShader(ar,"DebugMaterial",LoadDebugMaterial);
}
// clang-format on
} // namespace jpctracer::parse
