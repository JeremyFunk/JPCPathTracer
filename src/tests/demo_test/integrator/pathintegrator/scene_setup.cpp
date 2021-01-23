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

std::shared_ptr<std::vector<std::shared_ptr<IShape>>> generate_shapes() 
{
    std::shared_ptr<std::vector<std::shared_ptr<IShape>>> shapeList = std::make_shared<std::vector<std::shared_ptr<IShape>>>();

    auto color_blue = std::make_shared<ColorValueVec3>(ColorValueVec3({0.2,0.2,1}));
    auto color_red = std::make_shared<ColorValueVec3>(ColorValueVec3({1,0.2,0.2}));
    auto color_green = std::make_shared<ColorValueVec3>(ColorValueVec3({0.2,1,0.2}));
    auto color_white = std::make_shared<ColorValueVec3>(ColorValueVec3({1,1,1}));
    

    auto material_blue = std::make_shared<BasicMaterial>(color_blue,10);
    auto material_red = std::make_shared<BasicMaterial>(color_red,0);
    auto material_red_glossy = std::make_shared<GlossyMaterial>(color_red,0);
    auto material_green = std::make_shared<BasicMaterial>(color_green,0);  
    auto material_white = std::make_shared<BasicMaterial>(color_white,100);
    auto material_white_glossy = std::make_shared<GlossyMaterial>(color_white,0.05);

    Transformation plain_transformation (Vec3(0, 0, -8), Vec3(0, 0, 0), Vec3(1.3,.9,1));
    auto mesh = LoadMesh(PlainPath(), material_white_glossy, std::make_shared<Transformation>(plain_transformation), shapeList);
    //auto sphere = std::make_shared<Sphere>(Vec3{0,0,-8},2, material_white);
    //shapeList->push_back(sphere);
    
    //Transformation monkey_transformation(Vec3(-3, 0, -5), Vec3(0, 0, 0), Vec3(.8,.8,.8));
    //auto monkeyMesh = LoadMesh(MonkeyPath(), material_green, std::make_shared<Transformation>(monkey_transformation), shapeList);
    //Transformation icosphere_transformation(Vec3(-3, 0, -4), Vec3(0, 0, 0), Vec3(.8,.8,.8));
    //auto icosphereMesh = LoadMesh(IcospherePath(), material_green, std::make_shared<Transformation>(icosphere_transformation), shapeList);
    auto sphere1 = std::make_shared<Sphere>(Vec3(-3, 0, -4),1,material_green);
    shapeList->push_back(sphere1);

    return shapeList;
}

std::shared_ptr<std::vector<std::shared_ptr<ILight>>> generate_lights() 
{
    std::shared_ptr<std::vector<std::shared_ptr<ILight>>> lightList = std::make_shared<std::vector<std::shared_ptr<ILight>>>();

    lightList->push_back(std::make_shared<PointLight>(Vec3(0, 0, -7), RGBSpectrum::FromRGB({1000,1000,1000})));
    //lightList->push_back(std::make_shared<PointLight>(Vec3(2, 0, 5), RGBSpectrum::FromRGB({1000,1000,200})));

    return lightList;
}

}