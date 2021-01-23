#include "gtest/gtest.h"
#include <memory>
#include <vector>
#include "core/Bsdf.h"
#include "core/ILight.h"
#include "core/IMaterial.h"
#include "core/IShape.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "integrators/LightIntegrator.h"
#include "scenes/BVHScene.h"
#include "shapes/Sphere.h"
#include "materials/BasicMaterial.h"
#include "lights/PointLight.h"
namespace jpc_tracer {
    TEST(direct_light, singleray)
    {
        auto color = std::make_shared<ColorValueVec3>(ColorValueVec3({1,1,1}));
        auto integrator = std::make_shared<LightIntegrator>();

        auto material = std::make_shared<BasicMaterial>(color,0);
        auto lights = std::make_shared<std::vector<std::shared_ptr<ILight>>>();
        auto shapes = std::make_shared<std::vector<std::shared_ptr<IShape>>>();
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
        auto sphere = std::make_shared<Sphere>(Vec3{0,0,-4},1,material);
        shapes->push_back(sphere);
        auto point_light = std::make_shared<PointLight>(Vec3{2,0,-2},Spectrum::FromRGB({100,100,100}));
        lights->push_back(point_light);

        auto scene = std::make_shared<BVHScene>(shapes,lights);
        integrator->Init(scene);
        //BsdfMemoryPtr memory = CreateBsdfMemory;
        BsdfMemoryInfo info = scene->GetBsdfInfo();
        
        BsdfMemoryPtr memory = CreateBsdfMemory(info);
        Ray ray({0,0,0},{0,0,-1});
        SpectrumPasses spec ;//= integrator->Integrate(ray, nullptr,memory );
        Vec3 rgb = spec.GetCombined().ToRGB();
        std::cout<<rgb[0]<<","<<rgb[1]<<","<<rgb[2]<<std::endl;
    }
}