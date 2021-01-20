#include "BVHScene.h"
#include "accelerators/BVHAccel.h"
#include "core/Bsdf.h"
#include "core/IMaterial.h"
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
    }

    std::optional<SurfaceProperties> BVHScene::Intersect(const Ray& ray) const{

       
        std::optional<core::IntersectionData> closestInteraction = _bvh_tree->Traversal(ray);

        if(!closestInteraction)
            return std::nullopt;

        return std::make_optional<SurfaceProperties>(closestInteraction->Shape->GetSurfaceProperties(ray, closestInteraction.value()));
    }
    std::optional<Prec> BVHScene::IntersectionDistance(const Ray& ray) const{
        
        std::optional<core::IntersectionData> closestInteraction = _bvh_tree->Traversal(ray);

        if(!closestInteraction)
            return std::nullopt;
        
        return std::make_optional<core::Prec>(closestInteraction->Distance);
    }
    std::vector<std::shared_ptr<ILight>> BVHScene::GetLights() const{
        return *_lightList;
    }
    
    BsdfMemoryInfo BVHScene::GetBsdfInfo() const
    {
        BsdfMemoryInfo bsdf_info = {0,0};
        std::vector<std::shared_ptr<const core::IMaterial>> materials;
        for(const auto& shape : *_shapeList)
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