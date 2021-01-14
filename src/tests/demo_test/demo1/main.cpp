#include "core/Linalg.h"
#include "renderers/BasicRenderer.h"
#include "samplers/StratifiedSampler.h"
#include "cameras/ProjectionCamera.h"
#include "scenes/BasicScene.h"
#include "integrators/BasicIntegrator.h"
#include "integrators/LightIntegrator.h"
#include "filters/GaussianFilter.h"
#include "films/BasicFilm.h"
#include "textures/Texture.h"
#include "utilities/textureLoader.h"
#include <memory>



#include "Eigen/Dense"
#include "Eigen/src/Core/Matrix.h"
#include "core/Linalg.h"

int main()
{

    auto tr = core::LinAlg::GetTransformationMatrix(core::Vec3(2, 2, -4), core::Vec3(-90, 0, 0), core::Vec3(2, 2, 2));
    auto id = core::Mat4x4::Identity();
    auto trid = tr*id;

    core::Vec3 n(0, 0, -1);


    int width = 1080;
    int height = 720;
    auto sampler = std::make_shared<samplers::StratifiedSampler>();
    auto camera = std::make_shared<cameras::ProjectionCamera>(width,height,1);
    auto scene = std::make_shared<scenes::BasicScene>();
    auto integrator = std::make_shared<integrators::LightIntegrator>();
    integrator->Init(scene);
    auto filter = std::make_shared<filters::GaussianFilter>(0.5);
    auto film = std::make_shared<films::BasicFilm>(width,height);
    auto renderer = std::make_shared<renderers::BasicRenderer>(1);
    renderer->Init(sampler, camera, scene, integrator, filter, film);
    renderer->Render();
    film->WriteImage("demo1.png");
}