#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/utilities/MeshIO.h"
#include "jpc_tracer/plugins/Plugins.h"
#include "jpc_tracer/plugins/cameras/ProjectionCamera.h"
#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"
#include <memory>

int main()
{
    using namespace jpctracer;
    Logger::Init();
    using sampler_t = decltype(sampler::StratifiedSampler2());
    std::unique_ptr<ISampler> sampler = std::make_unique<sampler_t>(sampler::StratifiedSampler2());

    std::unique_ptr<ICamera> camera = std::make_unique<camera::ProjectionCamera>(1);

    std::unique_ptr<IIntegrator> integrator = std::make_unique<DirectLightIntegrator>(4,4);
    
    JPCRenderer renderer(std::move(sampler),std::move(camera),std::move(integrator));
    renderer.TileSize = 64;
    renderer.ShouldMultiThread = false;
    
    renderer.MaterialLib.Register(
        "Default",
        [](MaterialSettings settings)
        {
            
            auto bsdf_glossy = ShaderBind(GlossyBsdf, FromRGB({1,1,1}),settings.GetValue("roughness"));
            auto bsdf_diffuse = ShaderBind(DiffuseBsdf,settings.GetColor("color"));
            return ShaderBind(MixBsdf,bsdf_glossy,bsdf_diffuse,settings.GetValue("glossy_influence"));
        },
        {
            {"color",FromRGB({1,1,0})},
            {"roughness",0.f},
            {"glossy_influence", 0.2f }
        }
        );

    auto shader = renderer.MaterialLib.Get("Default");
    std::cout << "Set roughness\n";
    float roughness = 0;
    float glossy_influence = 0;
    std::cin >> roughness;
    std::cout << "Set glossy influence\n";
    std::cin >> glossy_influence;
    shader->SetValue("Roughness", roughness);
    shader->SetValue("glossy_influence", glossy_influence);
    //chris
    auto triangle = CreateTriangle({-1,1,-2}, {1,-1,-2}, {1,1,-2});

    triangle->MaterialSlots[0] = shader;

    renderer.Draw(triangle);
    renderer.LightsLib.AddPointLight({0,0,0}, FromRGB({10,10,10}));

    //Chris
    renderer.Render(500, 300, "");

    return 0;
}