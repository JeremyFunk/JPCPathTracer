#include "color/ColorValue.h"
#include "core/Base.h"
#include "core/JPCPathtracerApi.h"
#include "core/Transformation.h"
#include "jpctracer/JPCDefaultApi.h"
#include "lights/PointLight.h"
#include "materials/BasicMaterial.h"
#include "materials/GlossyMaterial.h"
#include "materials/GlossyDiffMaterial.h"
#include "utilities/objLoader.h"

namespace jpc_tracer {

std::string ProjectPath()
{
    //return "E:\\dev\\pathTrace\\JPCPathTracer\\";
    //return "H:\\dev\\path-tracing\\JPCPathTracer\\";
    return "/home/chris/Dev/path_tracing/JPCPathTracer/";
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


    class Demo1Api : public JPCDefaultApi
    {
    public:
        Demo1Api(DefaultSettings settings) : JPCDefaultApi(settings) {}
        virtual Ref<std::vector<Ref<IShape>>> GetShapes()
        {
            Ref<std::vector<Ref<IShape>>> shapeList = MakeRef<std::vector<Ref<IShape>>>();

            auto color_blue = MakeRef<ColorValueVec3>(ColorValueVec3({0.2,0.4,0.8}));
            auto color_red = MakeRef<ColorValueVec3>(ColorValueVec3({1,0.2,0.2}));
            auto color_green = MakeRef<ColorValueVec3>(ColorValueVec3({0.2,1,0.2}));
            auto color_white = MakeRef<ColorValueVec3>(ColorValueVec3({1,1,1}));
            

            auto mix_material = MakeRef<GlossyDiffMaterial>(color_blue,color_white,0.01,0.5);
            auto glossy_material = MakeRef<GlossyMaterial>(color_white,0.01);
            auto diffuse_material = MakeRef<BasicMaterial>(color_white,0);
            
            Transformation monkey_transformation(Vec3(0, -0, -8), Vec3(34.0494, 30, -20), Vec3(2,2,2));
            auto monkeyMesh = LoadMesh(MonkeyPath(), mix_material, MakeRef<Transformation>(monkey_transformation), shapeList);
            
            Transformation plain_transformation (Vec3(0, -1, -4), Vec3(90, 0, 0), Vec3(20,20,1));
            auto mesh = LoadMesh(PlainPath(), glossy_material, MakeRef<Transformation>(plain_transformation), shapeList);
            return shapeList;
        }
        virtual Ref<std::vector<Ref<ILight>>> GetLights()
        {
            Ref<std::vector<Ref<ILight>>> lightList = MakeRef<std::vector<Ref<ILight>>>();

            lightList->push_back(MakeRef<PointLight>(Vec3(0, 10, 0), RGBSpectrum::FromRGB({10000,10000,10000})));
            //lightList->push_back(MakeRef<PointLight>(Vec3(2, 0, 5), RGBSpectrum::FromRGB({1000,1000,200})));
        
            return lightList;
        }

    };

    

}
int main()
{
    using namespace jpc_tracer;
    DefaultSettings settings;
    settings.SkyColor = {0.4,0.4,0.4};
    settings.SampleCount = 64*64;
    settings.MaxRayDepth = 5;
    Demo1Api api(settings);
    auto renderer = api.FullRenderEngine();
    renderer->Render();
    renderer->WriteImage("mix_material.png");
}