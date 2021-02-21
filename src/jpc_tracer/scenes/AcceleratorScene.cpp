#include "AcceleratorScene.h"
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


namespace jpc_tracer
{

    AcceleratorScene::AcceleratorScene(Ref<IAccelerator>& accel, const Ref<std::vector<Ref<IShape>>>& shapeList, const Ref<std::vector<Ref<ILight>>>& lightList)
        : _shapeList(shapeList),
          _lightList(lightList),
          _accel(accel)
    {
    }

    std::optional<SurfaceProperties> AcceleratorScene::Intersect(const Ray& ray) const{

        std::optional<IntersectionData> closestInteraction = _accel->Traversal(ray);

        if(!closestInteraction)
            return std::nullopt;

        return std::make_optional<SurfaceProperties>(closestInteraction->Shape->GetSurfaceProperties(ray, closestInteraction.value()));
    }
    std::optional<Prec> AcceleratorScene::IntersectionDistance(const Ray& ray) const{
        
        std::optional<IntersectionData> closestInteraction = _accel->Traversal(ray);

        if(!closestInteraction)
            return std::nullopt;
        
        return std::make_optional<Prec>(closestInteraction->Distance);
    }
    std::vector<Ref<ILight>> AcceleratorScene::GetLights() const{
        return *_lightList;
    }
    
    BsdfMemoryInfo AcceleratorScene::GetBsdfInfo() const
    {
        BsdfMemoryInfo bsdf_info = {0,0};
        std::vector<Ref<const IMaterial>> materials;
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