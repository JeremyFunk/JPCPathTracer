#include "BVHScene.h"
#include "accelerators/BVHAccel.h"
#include "materials/BasicMaterial.h"
#include "core/IShape.h"
#include "core/SurfaceInteraction.h"
#include "utilities/textureLoader.h"
#include "textures/Texture.h"
#include "shapes/TriangleMesh.h"
#include <memory.h>
#include <memory>
#include <optional>
#include <string>
#include "utilities/objLoader.h"
#include "core/Transformation.h"


namespace scenes
{

    BVHScene::BVHScene(const std::shared_ptr<std::vector<std::shared_ptr<IShape>>>& shapeList, const std::shared_ptr<std::vector<std::shared_ptr<ILight>>>& lightList)
        : _shapeList(shapeList),
          _lightList(lightList),
          _bvh_tree(std::make_shared<accel::BVHAccel>(shapeList, 1))
    {
        //_bvh_tree->BuildBVH();

        // std::shared_ptr<IMaterial> material_blue = std::make_shared<materials::BasicMaterial>(
        //     RGBSpectrum::FromRGB({0,1,1}),1);
            
        
        // std::shared_ptr<IMaterial> material_red = std::make_shared<materials::BasicMaterial>(
        //     RGBSpectrum::FromRGB({0,0,1}),0);

        // std::shared_ptr<IMaterial> material_white = std::make_shared<materials::BasicMaterial>(
        //     RGBSpectrum::FromRGB({1,1,1}),0); 

        // auto transformation = std::make_shared<core::Transformation>(core::Vec3(0, 0, -8), core::Vec3(0, 0, 0), core::Vec3(1,1,1));

        // std::string path = "E:\\dev\\pathTrace\\local\\jcp-path-tracer\\resource\\Susan.obj";
        // auto mesh = utilities::LoadMesh(path, material_white, transformation, shapeList);

        // shapeList->push_back(std::make_shared<shapes::Sphere>(Vec3(2,0,-5), 1, material_red));
        // shapeList.push_back(std::make_shared<shapes::Sphere>(Vec3(-2,0,-5),1,nullptr));
        // shapeList.push_back(std::make_shared<shapes::Sphere>(Vec3(0,0,-9),1,nullptr));

        // bvh_tree = std::make_shared<accel::BVHAccel>(shapeList, 1);

        // bvh_tree->BuildBVH();

        // lightList->push_back(std::make_shared<lights::PointLight>(Vec3(-8, 0, 0), RGBSpectrum::FromRGB({200,0,0})));
        // lightList->push_back(std::make_shared<lights::PointLight>(Vec3(8, 0, 0), RGBSpectrum::FromRGB({0,0,200})));
        // lightList->push_back(std::make_shared<lights::PointLight>(Vec3(0, 0, 0), RGBSpectrum::FromRGB({0,20,0})));
        // lightList.push_back(std::make_shared<lights::PointLight>(Vec3(0, 0, -5), RGBSpectrum::FromRGB({0,10,0})));

        //lightList.push_back(std::make_shared<lights::DirectionalLight>(Vec3(0, 0, -1).normalized(), RGBSpectrum::FromRGB({0,.2,0})));
        //lightList.push_back(std::make_shared<lights::DirectionalLight>(Vec3(0, 0, 1).normalized(), RGBSpectrum::FromRGB({0,.2,0})));
    }

    std::optional<SurfaceProperties> BVHScene::Intersect(const Ray& ray) const{

        // std::optional<core::IntersectionData> closestInteraction = std::nullopt;

        // for(auto shape : shapeList){
        //     auto surfaceInteraction = shape->Intersect(ray);
        //     if(surfaceInteraction)
        //     {
        //         if(!closestInteraction.has_value() || closestInteraction->Distance > surfaceInteraction->Distance){
        //             closestInteraction = surfaceInteraction;
        //         }
        //     }
        // }
        
        // if(! closestInteraction)
        //     return std::nullopt;

        std::optional<core::IntersectionData> closestInteraction = _bvh_tree->Traversal(ray);

        if(!closestInteraction)
            return std::nullopt;

        return std::make_optional<SurfaceProperties>(closestInteraction->Shape->GetSurfaceProperties(ray, closestInteraction.value()));
    }
    std::optional<Prec> BVHScene::IntersectionDistance(const Ray& ray) const{
        // std::optional<core::IntersectionData> closestInteraction = std::nullopt;

        // for(auto shape : shapeList){
        //     auto surfaceInteraction = shape->Intersect(ray);
        //     if(surfaceInteraction){
        //         if(!closestInteraction.has_value() || closestInteraction->Distance > surfaceInteraction->Distance){
        //             closestInteraction = surfaceInteraction;
        //         }
        //     }
        // }

        std::optional<core::IntersectionData> closestInteraction = _bvh_tree->Traversal(ray);

        if(!closestInteraction)
            return std::nullopt;
        
        return std::make_optional<core::Prec>(closestInteraction->Distance);
    }
    std::vector<std::shared_ptr<ILight>> BVHScene::GetLights() const{
        return *_lightList;
    }
}