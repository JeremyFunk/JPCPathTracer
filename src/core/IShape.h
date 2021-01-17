#pragma once
#include "Ray.h"
#include "core/IMaterial.h"
#include "core/IShape.h"
#include "core/SurfaceProperties.h"
#include "core/Linalg.h"
#include "core/Bounds3D.h"
#include <memory>
#include <optional>
#include <vector>

namespace core {
    
    class IntersectionData;
    class IShape
    {
    public:
        virtual std::optional<IntersectionData> Intersect(const Ray& ray) const= 0;
        virtual const SurfaceProperties GetSurfaceProperties(const Ray& ray,const IntersectionData& data) const = 0;
        virtual core::Bounds3D<core::Prec> WorldBoundary() const = 0;
        virtual std::vector<std::shared_ptr<const IMaterial>> GetMaterials() const = 0;
    };

    struct IntersectionData
    {
        Vec2 UV;
        float Distance;
        IShape* Shape ;
    };
}