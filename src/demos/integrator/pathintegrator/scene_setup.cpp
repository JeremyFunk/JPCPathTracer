#include "scene_setup.h"
#include "shapes/Sphere.h"
#include "materials/BasicMaterial.h"
#include "materials/GlossyMaterial.h"
#include <memory>
#include "utilities/objLoader.h"
#include "lights/PointLight.h"
#include "textures/Texture.h"
#include "utilities/textureLoader.h"
#include "shapes/Sphere.h"
namespace jpc_tracer {


std::string ProjectPath()
{
    //return "E:\\dev\\pathTrace\\local\\jcp-path-tracer\\resource\\";
    return "H:\\dev\\path-tracing\\JPCPathTracer\\";
    //return "/home/chris/Dev/path_tracing/JPCPathTracer/";
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

Ref<std::vector<Ref<IShape>>> generate_shapes() 
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
    auto material_white = MakeRef<BasicMaterial>(color_white,0);
    auto material_white_glossy = MakeRef<GlossyMaterial>(color_white,0.02);

    //Transformation plain_transformation (Vec3(0, 0, -8), Vec3(0, 0, 0), Vec3(1.3,.9,1));
    //auto mesh = LoadMesh(PlainPath(), material_white_glossy, MakeRef<Transformation>(plain_transformation), shapeList);
    //auto sphere = MakeRef<Sphere>(Vec3{0,0,-8},2, material_white);
    //shapeList->push_back(sphere);
    
    //Transformation monkey_transformation(Vec3(-3, 0, -5), Vec3(0, 0, 0), Vec3(.8,.8,.8));
    //auto monkeyMesh = LoadMesh(MonkeyPath(), material_green, MakeRef<Transformation>(monkey_transformation), shapeList);
    //Transformation icosphere_transformation(Vec3(-3, 0, -4), Vec3(0, 0, 0), Vec3(.8,.8,.8));
    //auto icosphereMesh = LoadMesh(IcospherePath(), material_green, MakeRef<Transformation>(icosphere_transformation), shapeList);
    //auto sphere1 = MakeRef<Sphere>(Vec3(-3, 0, -4),1,material_green);
    //shapeList->push_back(sphere1);

    Transformation monkey_transformation(Vec3(-3, -2, -5), Vec3(0, 0, 0), Vec3(1,1,1));
    auto monkeyMesh = LoadMesh(MonkeyPath(), material_red, MakeRef<Transformation>(monkey_transformation), shapeList);
    
    Transformation plain_transformation (Vec3(0, -2, -5), Vec3(90, 0, 0), Vec3(1.3,.9,1));
    auto mesh = LoadMesh(PlainPath(), material_white_glossy, MakeRef<Transformation>(plain_transformation), shapeList);

    return shapeList;
}

Ref<std::vector<Ref<ILight>>> generate_lights() 
{
    Ref<std::vector<Ref<ILight>>> lightList = MakeRef<std::vector<Ref<ILight>>>();

    lightList->push_back(MakeRef<PointLight>(Vec3(0, 5, 0), RGBSpectrum::FromRGB({10000,10000,10000})));
    //lightList->push_back(MakeRef<PointLight>(Vec3(2, 0, 5), RGBSpectrum::FromRGB({1000,1000,200})));

    return lightList;
}

}