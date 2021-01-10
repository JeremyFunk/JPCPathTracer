#include "BasicScene.h"
#include "core/IShape.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include "core/SurfaceInteraction.h"
#include "shapes/TriangleMesh.h"
#include <memory.h>
#include <optional>
#include "textures/Texture.h"
#include "utilities/textureLoader.h"
#include <string>
#include "utilities/objLoader.h"
#include "materials/BasicMaterial.h"
//#include "materials/GlossyMaterial.h"
#include "color/ColorValue.h"


namespace scenes
{

    BasicScene::BasicScene() : shapeList(std::make_shared<std::vector<std::shared_ptr<IShape>>>()){

        //auto texture = utilities::textureLoader::LoadColorValueTexture("C:\\Users\\Jerem\\Desktop\\Image1.jpg");
        auto texture = utilities::textureLoader::LoadColorValueTexture("F:\\blenderhilfe vol 1\\blenderHilfe Textures Vol1 2\\Boden\\Boden - Holz\\Boden_Parket_001_COL2_tiled.jpg");


         std::shared_ptr<IMaterial> material_blue = std::make_shared<materials::BasicMaterial>(
            std::make_shared<color::ColorValueVec3>(color::ColorValueVec3({0,1,0})),1);
            
        std::shared_ptr<IMaterial> material_red = std::make_shared<materials::BasicMaterial>(
            std::make_shared<color::ColorValueVec3>(color::ColorValueVec3({0,1,0})),1);
            
        std::shared_ptr<IMaterial> material_pink = std::make_shared<materials::BasicMaterial>(
            std::make_shared<color::ColorValueVec3>(color::ColorValueVec3({0,1,0})),1);

        std::shared_ptr<IMaterial> material_white = std::make_shared<materials::BasicMaterial>(
            texture,1);

        core::Transformation m (core::Vec3(0, 0, -8), core::Vec3(0, 0, 0), core::Vec3(1.3,.9,1));
        //E:\\dev\\pathTrace\\local\\jcp-path-tracer\\resource\\plain.obj
        //std::string path = "C:\\Users\\Jerem\\Desktop\\plain.obj";
        //Chris
        std::string path = "H:\\dev\\path-tracing\\jcp-path-tracer\\resource\\plain.obj";
       
        auto mesh = utilities::LoadMesh(path,material_pink, std::make_shared<core::Transformation>(m), shapeList);

        // shapes::Triangle triangle(mesh, idxs); 
        
        //shapeList.push_back(std::make_shared<shapes::Triangle>(triangle));
        
       
       
        core::Transformation mMonkey(core::Vec3(2, 0, -5), core::Vec3(0, 0, 0), core::Vec3(.8,.8,.8));

        //E:\\dev\\pathTrace\\local\\jcp-path-tracer\\resource\\Susan.obj
        //std::string monkey = "C:\\Users\\Jerem\\Desktop\\Icosphere.obj";

        std::string monkey = "H:\\dev\\path-tracing\\jcp-path-tracer\\resource\\Susan.obj";
        auto monkeyMesh = utilities::LoadMesh(monkey,material_red, std::make_shared<core::Transformation>(mMonkey), shapeList);



        //shapeList->push_back(std::make_shared<shapes::Sphere>(Vec3(0,0,-15),6, material_red));
        //shapeList.push_back(std::make_shared<shapes::Sphere>(Vec3(0,0,-9),1, material_white));
        //lightList.push_back(std::make_shared<lights::PointLight>(Vec3(-8, 0, -5), RGBSpectrum::FromRGB({100,0,0})));
        lightList.push_back(std::make_shared<lights::PointLight>(Vec3(3, 0, -3), RGBSpectrum::FromRGB({0,0,50})));
        //lightList.push_back(std::make_shared<lights::PointLight>(Vec3(0, 0, -5), RGBSpectrum::FromRGB({5,5,5})));

        //lightList.push_back(std::make_shared<lights::DirectionalLight>(Vec3(0, 0, -1).normalized(), RGBSpectrum::FromRGB({0,.2,0})));
        //lightList.push_back(std::make_shared<lights::DirectionalLight>(Vec3(0, 0, 1).normalized(), RGBSpectrum::FromRGB({0,.2,0})));
    }

    std::optional<SurfaceProperties> BasicScene::Intersect(const Ray& ray) const{

        std::optional<core::IntersectionData> closestInteraction = std::nullopt;

        for(auto shape : *shapeList){
            auto surfaceInteraction = shape->Intersect(ray);
            if(surfaceInteraction)
            {
                if(!closestInteraction.has_value() || closestInteraction->Distance > surfaceInteraction->Distance){
                    closestInteraction = surfaceInteraction;
                }
            }
        }
        
        if(! closestInteraction)
            return std::nullopt;

        return std::make_optional<SurfaceProperties>(closestInteraction->Shape->GetSurfaceProperties(ray,closestInteraction.value()));
    }
    std::optional<Prec> BasicScene::IntersectionDistance(const Ray& ray) const{

        std::optional<core::IntersectionData> closestInteraction = std::nullopt;

        for(auto shape : *shapeList){
            auto surfaceInteraction = shape->Intersect(ray);
            if(surfaceInteraction){
                if(!closestInteraction.has_value() || closestInteraction->Distance > surfaceInteraction->Distance){
                    closestInteraction = surfaceInteraction;
                }
            }
        }
        
        return std::make_optional<core::Prec>(closestInteraction->Distance);
    }
    std::vector<std::shared_ptr<ILight>> BasicScene::GetLights() const{
        return lightList;
    }
}