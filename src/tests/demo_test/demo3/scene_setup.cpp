#include "scene_setup.h"
#include "shapes/Sphere.h"
#include "materials/BasicMaterial.h"
//#include "materials/GlossyMaterial.h"
#include <memory>
#include "utilities/objLoader.h"
#include "lights/PointLight.h"
#include "textures/Texture.h"
#include "utilities/textureLoader.h"


std::string PlainPath()
{
        
    //return "E:\\dev\\pathTrace\\local\\jcp-path-tracer\\resource\\plain.obj";
    //return "C:\\Users\\Jerem\\Desktop\\plain.obj";
    return "H:\\dev\\path-tracing\\JPCPathTracer\\resource\\plain.obj";
}

std::string MonkeyPath()
{
    //return "E:\\dev\\pathTrace\\local\\jcp-path-tracer\\resource\\Susan.obj";
    //return "C:\\Users\\Jerem\\Desktop\\Icosphere.obj";
    return "H:\\dev\\path-tracing\\JPCPathTracer\\resource\\Susan.obj";
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
    auto material_white = std::make_shared<materials::BasicMaterial>(color_white,0);  
    

    core::Transformation plain_transformation (core::Vec3(0, 0, -8), core::Vec3(0, 0, 0), core::Vec3(1.3,.9,1));
    auto mesh = utilities::LoadMesh(PlainPath(), material_red, std::make_shared<core::Transformation>(plain_transformation), shapeList);

    
    core::Transformation monkey_transformation(core::Vec3(2, 0, -5), core::Vec3(0, 0, 0), core::Vec3(.8,.8,.8));
    auto monkeyMesh = utilities::LoadMesh(MonkeyPath(), material_white, std::make_shared<core::Transformation>(monkey_transformation), shapeList);


    return shapeList;
}

std::shared_ptr<std::vector<std::shared_ptr<core::ILight>>> generate_lights() 
{
    std::shared_ptr<std::vector<std::shared_ptr<core::ILight>>> lightList = std::make_shared<std::vector<std::shared_ptr<core::ILight>>>();

    lightList->push_back(std::make_shared<lights::PointLight>(core::Vec3(3, 0, -3), core::RGBSpectrum::FromRGB({100,100,100})));


    return lightList;
}
