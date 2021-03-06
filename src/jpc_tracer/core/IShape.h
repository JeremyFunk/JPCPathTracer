#pragma once
#include"Base.h"
#include "Ray.h"
#include "core/IMaterial.h"
#include "core/IShape.h"
#include "core/SurfaceProperties.h"
#include "core/Linalg.h"
#include "core/Bounds3D.h"
#include <memory>
#include <optional>
#include <vector>

namespace jpc_tracer {
    
    class IntersectionData;
    class IShape
    {
    public:
        //Shape
        virtual std::optional<IntersectionData> Intersect(const Ray& ray) const= 0;
        virtual const SurfaceProperties GetSurfaceProperties(const Ray& ray,const IntersectionData& data) const = 0;
        virtual Bounds3D<Prec> WorldBoundary() const = 0;
        virtual Vec3 getCenter() const = 0;
        virtual std::vector<Ref<const IMaterial>> GetMaterials() const = 0;
        virtual ~IShape() {}
    };

    struct IntersectionData
    {
        Vec2 UV;
        float Distance;
        IShape* Shape ;
    };
}