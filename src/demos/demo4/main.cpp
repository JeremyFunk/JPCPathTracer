#include "core/Bsdf.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "core/SurfaceProperties.h"
#include "integrators/LightIntegrator.h"
#include "renderers/BasicRenderer.h"
#include "samplers/StratifiedSampler.h"
#include "cameras/ProjectionCamera.h"
#include "scenes/LBVHScene.h"
#include "scenes/BVHScene.h"
#include "scenes/BasicScene.h"
#include "integrators/PathIntegrator.h"
#include "integrators/DebugBsdfIntegrator.h"
#include "filters/GaussianFilter.h"
#include "films/BasicFilm.h"
#include <iostream>
#include <memory>
#include "color/ColorValue.h"
#include "core/Base.h"
#include "core/JPCPathtracerApi.h"
#include "core/Transformation.h"
#include "jpctracer/JPCDefaultApi.h"
#include "lights/PointLight.h"
#include "materials/BasicMaterial.h"
#include "materials/GlossyMaterial.h"
#include "utilities/objLoader.h"

namespace jpc_tracer {

    std::string ProjectPath()
    {
        return "E:\\dev\\pathTrace\\JPCPathTracer\\";
        //return "H:\\dev\\path-tracing\\JPCPathTracer\\";
        // return "/home/chris/Dev/path_tracing/JPCPathTracer/";
    }

    std::string PlainPath()
    {
        return ProjectPath()+"resource/plain.obj";
    }

    std::string MonkeyPath()
    {

        return ProjectPath()+"resource/Susan.obj";
    }

    std::string IcospherePath()
    {
        return ProjectPath() + "resource/icosphere.obj";
    }

    class Demo4Api : public JPCDefaultApi
    {
    public:
        Demo4Api(DefaultSettings settings) : JPCDefaultApi(settings) {}
        virtual Ref<std::vector<Ref<IShape>>> GetShapes() override
        {
            Ref<std::vector<Ref<IShape>>> shapeList = MakeRef<std::vector<Ref<IShape>>>();

            auto color_blue = MakeRef<ColorValueVec3>(ColorValueVec3({0.2,0.2,1}));
            auto color_red = MakeRef<ColorValueVec3>(ColorValueVec3({1,0.2,0.2}));
            auto color_green = MakeRef<ColorValueVec3>(ColorValueVec3({0.2,1,0.2}));
            auto color_white = MakeRef<ColorValueVec3>(ColorValueVec3({1,1,1}));
            
            auto material_blue = MakeRef<BasicMaterial>(color_blue,10);
            auto material_red = MakeRef<BasicMaterial>(color_red,0);
            auto material_red_glossy = MakeRef<GlossyMaterial>(color_red,0);
            auto material_green = MakeRef<BasicMaterial>(color_green,0);  
            auto material_white = MakeRef<BasicMaterial>(color_white,100);
            auto material_white_glossy = MakeRef<GlossyMaterial>(color_white,0.1);

            Transformation plain_transformation (Vec3(0, 0, -8), Vec3(0, 0, 0), Vec3(1.3,.9,1));
            auto mesh = LoadMesh(PlainPath(), material_white_glossy, MakeRef<Transformation>(plain_transformation), shapeList);

            return shapeList;
        }

        virtual Ref<std::vector<Ref<ILight>>> GetLights() override
        {
            Ref<std::vector<Ref<ILight>>> lightList = MakeRef<std::vector<Ref<ILight>>>();

            lightList->push_back(MakeRef<PointLight>(Vec3(-1, 0, -2), RGBSpectrum::FromRGB({2000,2000,2000})));
            //lightList->push_back(MakeRef<PointLight>(Vec3(2, 0, 5), RGBSpectrum::FromRGB({1000,1000,200})));
        
            return lightList;
        }

        virtual Scope<IScene> GetScene() override
        {
            return MakeScope<LBVHScene>(GetShapes(), GetLights());
            return MakeScope< BVHScene>(GetShapes(), GetLights());
        }

    };

}

int main()
{
    using namespace jpc_tracer;
    DefaultSettings settings;
    Demo4Api api(settings);
    auto renderer = api.FullRenderEngine();
    renderer->Render();
    renderer->WriteImage("demo4.png");
}