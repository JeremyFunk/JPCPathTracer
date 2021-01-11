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

std::string ProjectPath()
{
    //return "E:\\dev\\pathTrace\\local\\jcp-path-tracer\\resource\\";
    return "H:\\dev\\path-tracing\\JPCPathTracer\\";
}

std::string PlainPath()
{
    return ProjectPath()+"resource\\plain.obj";
}

std::string MonkeyPath()
{

    return ProjectPath()+"resource\\Susan.obj";
}

std::string IcospherePath()
{
    return ProjectPath() + "resource\\icosphere.obj";
}

std::shared_ptr<std::vector<std::shared_ptr<core::IShape>>> generate_shapes() 
{
    std::shared_ptr<std::vector<std::shared_ptr<core::IShape>>> shapeList = std::make_shared<std::vector<std::shared_ptr<core::IShape>>>();

    auto color_blue = std::make_shared<color::ColorValueVec3>(color::ColorValueVec3({0,0,1}));
    auto color_red = std::make_shared<color::ColorValueVec3>(color::ColorValueVec3({1,0,0}));
    auto color_green = std::make_shared<color::ColorValueVec3>(color::ColorValueVec3({0,1,0}));
    auto color_white = std::make_shared<color::ColorValueVec3>(color::ColorValueVec3({1,1,1}));
    

    auto material_blue = std::make_shared<materials::BasicMaterial>(color_blue,0);
    auto material_red = std::make_shared<materials::BasicMaterial>(color_red,0);
    auto material_green = std::make_shared<materials::BasicMaterial>(color_green,0);  
    auto material_white_glossy = std::make_shared<materials::GlossyMaterial>(color_white,0);
    auto material_white = std::make_shared<materials::BasicMaterial>(color_white,0);

    core::Transformation plain_transformation (core::Vec3(0, 0, -8), core::Vec3(0, 0, 0), core::Vec3(1.3,.9,1));
    auto mesh = utilities::LoadMesh(PlainPath(), material_red, std::make_shared<core::Transformation>(plain_transformation), shapeList);
    //auto sphere = std::make_shared<shapes::Sphere>(core::Vec3{0,0,-8},2, material_white);
    //shapeList->push_back(sphere);
    
    //core::Transformation monkey_transformation(core::Vec3(0, 0, -5), core::Vec3(0, 0, 0), core::Vec3(.8,.8,.8));
    //auto monkeyMesh = utilities::LoadMesh(MonkeyPath(), material_white_glossy, std::make_shared<core::Transformation>(monkey_transformation), shapeList);
    core::Transformation icosphere_transformation(core::Vec3(0, 0, -4), core::Vec3(0, 0, 0), core::Vec3(.8,.8,.8));
    auto icosphereMesh = utilities::LoadMesh(IcospherePath(), material_white_glossy, std::make_shared<core::Transformation>(icosphere_transformation), shapeList);


    return shapeList;
}

std::shared_ptr<std::vector<std::shared_ptr<core::ILight>>> generate_lights() 
{
    std::shared_ptr<std::vector<std::shared_ptr<core::ILight>>> lightList = std::make_shared<std::vector<std::shared_ptr<core::ILight>>>();

    lightList->push_back(std::make_shared<lights::PointLight>(core::Vec3(-3, 0, 0), core::RGBSpectrum::FromRGB({1000,1000,1000})));


    return lightList;
}
