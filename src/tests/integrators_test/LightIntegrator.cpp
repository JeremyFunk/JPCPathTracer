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
        auto color = MakeRef<ColorValueVec3>(ColorValueVec3({1,1,1}));
        auto integrator = MakeRef<LightIntegrator>();

        auto material = MakeRef<BasicMaterial>(color,0);
        auto lights = MakeRef<std::vector<Ref<ILight>>>();
        auto shapes = MakeRef<std::vector<Ref<IShape>>>();
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
        auto sphere = MakeRef<Sphere>(Vec3{0,0,-4},1,material);
        shapes->push_back(sphere);
        auto point_light = MakeRef<PointLight>(Vec3{2,0,-2},Spectrum::FromRGB({100,100,100}));
        lights->push_back(point_light);

        auto scene = MakeRef<BVHScene>(shapes,lights);
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