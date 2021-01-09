#include "scene_setup.h"
#include "shapes/Sphere.h"
#include "materials/BasicMaterial.h"
#include "materials/GlossyMaterial.h"
#include <memory>
#include "utilities/objLoader.h"
#include "lights/PointLight.h"
#include "textures/Texture.h"
#include "utilities/textureLoader.h"


std::shared_ptr<std::vector<std::shared_ptr<core::IShape>>> generate_shapes() 
{
    std::shared_ptr<std::vector<std::shared_ptr<core::IShape>>> shapeList = std::make_shared<std::vector<std::shared_ptr<core::IShape>>>();


    auto texture = utilities::textureLoader::LoadColorValueTexture("C:\\Users\\Jerem\\Desktop\\Image1.jpg");
    //auto texture = utilities::textureLoader::LoadColorValueTexture("F:\\blenderhilfe vol 1\\blenderHilfe Textures Vol1 2\\Boden\\Boden - Holz\\Boden_Parket_001_COL2_tiled.jpg");
    using namespace core;

    std::shared_ptr<IMaterial> material_blue = std::make_shared<materials::BasicMaterial>(
        std::make_shared<color::ColorValueVec3>(color::ColorValueVec3({0,1,0})),1);
        
    std::shared_ptr<IMaterial> material_red = std::make_shared<materials::GlossyMaterial>(
        Spectrum::FromRGB({.3,.3,.8}),.2);
    std::shared_ptr<IMaterial> material_pink = std::make_shared<materials::GlossyMaterial>(
        Spectrum::FromRGB({.3,.3,.8}),.05);

    std::shared_ptr<IMaterial> material_white = std::make_shared<materials::BasicMaterial>(
        texture,1);

    core::Transformation m (core::Vec3(0, 0, -8), core::Vec3(0, 0, 0), core::Vec3(1.3,.9,1));
    //E:\\dev\\pathTrace\\local\\jcp-path-tracer\\resource\\plain.obj
    //std::string path = "C:\\Users\\Jerem\\Desktop\\plain.obj";

    std::string path = "C:\\Users\\Jerem\\Desktop\\plain.obj";
    auto mesh = utilities::LoadMesh(path,material_pink, std::make_shared<core::Transformation>(m), shapeList);

    // shapes::Triangle triangle(mesh, idxs); 
    
    //shapeList.push_back(std::make_shared<shapes::Triangle>(triangle));
    
    
    
    core::Transformation mMonkey(core::Vec3(2, 0, -5), core::Vec3(0, 0, 0), core::Vec3(.8,.8,.8));

    //E:\\dev\\pathTrace\\local\\jcp-path-tracer\\resource\\Susan.obj
    std::string monkey = "C:\\Users\\Jerem\\Desktop\\Icosphere.obj";

    //std::string monkey = "H:\\dev\\path-tracing\\jcp-path-tracer\\resource\\Susan.obj";
    auto monkeyMesh = utilities::LoadMesh(monkey,material_red, std::make_shared<core::Transformation>(mMonkey), shapeList);














    
    // std::shared_ptr<core::IMaterial> material_red = std::make_shared<materials::BasicMaterial>(
    //     core::RGBSpectrum::FromRGB({0,0,1}),0);

    // std::shared_ptr<core::IMaterial> material_white = std::make_shared<materials::BasicMaterial>(
    //     core::RGBSpectrum::FromRGB({1,1,1}),0); 

    // auto transformation = std::make_shared<core::Transformation>(core::Vec3(0, 0, -8), core::Vec3(0, 0, 0), core::Vec3(1,1,1));

    // std::string path = "E:\\dev\\pathTrace\\local\\jcp-path-tracer\\resource\\Susan.obj";
    // auto mesh = utilities::LoadMesh(path, material_white, transformation, shapeList);

    // shapeList->push_back(std::make_shared<shapes::Sphere>(core::Vec3(-2,0,-5), 1, material_red));

    return shapeList;
}

std::shared_ptr<std::vector<std::shared_ptr<core::ILight>>> generate_lights() 
{
    std::shared_ptr<std::vector<std::shared_ptr<core::ILight>>> lightList = std::make_shared<std::vector<std::shared_ptr<core::ILight>>>();


    using namespace core;
    //shapeList->push_back(std::make_shared<shapes::Sphere>(Vec3(0,0,-15),6, material_red));
    //shapeList.push_back(std::make_shared<shapes::Sphere>(Vec3(0,0,-9),1, material_white));
    //lightList.push_back(std::make_shared<lights::PointLight>(Vec3(-8, 0, -5), RGBSpectrum::FromRGB({100,0,0})));
    lightList->push_back(std::make_shared<lights::PointLight>(Vec3(3, 0, -3), RGBSpectrum::FromRGB({0,0,50})));
    //lightList.push_back(std::make_shared<lights::PointLight>(Vec3(0, 0, -5), RGBSpectrum::FromRGB({5,5,5})));

    //lightList.push_back(std::make_shared<lights::DirectionalLight>(Vec3(0, 0, -1).normalized(), RGBSpectrum::FromRGB({0,.2,0})));
    //lightList.push_back(std::make_shared<lights::DirectionalLight>(Vec3(0, 0, 1).normalized(), RGBSpectrum::FromRGB({0,.2,0})));
    // lightList->push_back(std::make_shared<lights::PointLight>(core::Vec3(-8, 0, 0), core::RGBSpectrum::FromRGB({200,0,0})));
    // lightList->push_back(std::make_shared<lights::PointLight>(core::Vec3(8, 0, 0), core::RGBSpectrum::FromRGB({0,0,200})));

    return lightList;
}
