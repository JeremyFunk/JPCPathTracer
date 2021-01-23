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

    BasicScene::BasicScene(const std::shared_ptr<std::vector<std::shared_ptr<IShape>>>& shapeList, const std::shared_ptr<std::vector<std::shared_ptr<ILight>>>& light) 
    : _shape_list(shapeList), _light_list(light)
    {

    }

    std::optional<SurfaceProperties> BasicScene::Intersect(const Ray& ray) const{

        std::optional<core::IntersectionData> closestInteraction = std::nullopt;

        for(auto shape : *_shape_list){
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

        for(auto shape : *_shape_list){
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
        return *_light_list;
    }

    BsdfMemoryInfo BasicScene::GetBsdfInfo() const
    {
        BsdfMemoryInfo bsdf_info = {0,0};
        std::vector<std::shared_ptr<const core::IMaterial>> materials;
        for(const auto& shape : *_shape_list)
        {
            
            for(const auto& material: shape->GetMaterials())
            {
                if(std::find(materials.begin(), materials.end(), material) == materials.end())
                    materials.push_back(material);
            }
        }
        for(const auto& material: materials)
        {
            BsdfMemoryInfo material_info = material->GetBsdfInfo();
            bsdf_info.max_bsdf_count+=material_info.max_bsdf_count;
            bsdf_info.max_byte_size += material_info.max_byte_size;
        }
        return bsdf_info;
    }
}