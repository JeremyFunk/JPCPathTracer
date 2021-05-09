#include "Archive.h"
#include "ParseScene.h"
#include "jpc_tracer/core/Logger.h"
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>

bool RenderFromSceneDescription(YAML::Node& node)
{
    using sampler_t = decltype(jpctracer::sampler::StratifiedSamplerFast());
    std::unique_ptr<jpctracer::ISampler> sampler =
        std::make_unique<sampler_t>(jpctracer::sampler::StratifiedSamplerFast());

    std::unique_ptr<jpctracer::ICamera> camera = std::make_unique<jpctracer::camera::ProjectionCamera>(1);

    std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DirectLightIntegrator>(4, 32);
    // std::unique_ptr<jpctracer::IIntegrator> integrator = std::make_unique<jpctracer::DebugIntegrator>();

    jpctracer::JPCRenderer renderer(std::move(sampler), std::move(camera), std::move(integrator));

    if (!jpctracer::parse::LoadRenderer(node, renderer))
    {
        JPC_LOG_ERROR("Could not load renderer");
        return false;
    }
    int width = 500;
    int height = 300;
    std::string directory = "";

    jpctracer::parse::Load(node["width"], width);
    jpctracer::parse::Load(node["height"], height);
    jpctracer::parse::Load(node["result_dir"], directory);

    renderer.Render(width, height, directory);
    return true;
}

int main(int argc, char* argv[])
{
    jpctracer::Logger::Init();
    std::string filepath = "settings.yaml";
    if (argc >= 1)
        filepath = std::string{argv[1]};
    YAML::Node node;
    try
    {
        node = YAML::LoadFile(filepath);
    }
    catch (const YAML::Exception& e)
    {
        JPC_LOG_ERROR("Could not open file in {}", filepath);
        return -1;
    }

    RenderFromSceneDescription(node);
    return 0;
}