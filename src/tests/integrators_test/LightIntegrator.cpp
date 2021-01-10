#include "gtest/gtest.h"
#include <memory>
#include <vector>
#include "core/ILight.h"
#include "core/IMaterial.h"
#include "core/IShape.h"
#include "core/Linalg.h"
#include "core/MemoryArea.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "integrators/LightIntegrator.h"
#include "scenes/BVHScene.h"
#include "shapes/Sphere.h"
#include "materials/BasicMaterial.h"
#include "lights/PointLight.h"
namespace integrators {
    TEST(direct_light, singleray)
    {
        auto color = std::make_shared<color::ColorValueVec3>(color::ColorValueVec3({1,1,1}));
        auto integrator = std::make_shared<integrators::LightIntegrator>();

        auto material = std::make_shared<materials::BasicMaterial>(color,0);
        auto lights = std::make_shared<std::vector<std::shared_ptr<core::ILight>>>();
        auto shapes = std::make_shared<std::vector<std::shared_ptr<core::IShape>>>();
        /*
        Scene Layout
         z
         |
      -4 |     S
         |
      -2 |         L
         |     
       0 - - - C - - - - x
          -2   0   2 
        */
        auto sphere = std::make_shared<shapes::Sphere>(core::Vec3{0,0,-4},1,material);
        shapes->push_back(sphere);
        auto point_light = std::make_shared<lights::PointLight>(core::Vec3{2,0,-2},core::Spectrum::FromRGB({100,100,100}));
        lights->push_back(point_light);

        auto scene = std::make_shared<scenes::BVHScene>(shapes,lights);
        integrator->Init(scene);
        core::MemoryArea memory;
        core::Ray ray({0,0,0},{0,0,-1});
        core::SpectrumPasses spec = integrator->Integrate(ray, nullptr,memory );
        core::Vec3 rgb = spec.GetCombined().ToRGB();
        std::cout<<rgb[0]<<","<<rgb[1]<<","<<rgb[2]<<std::endl;
    }
}