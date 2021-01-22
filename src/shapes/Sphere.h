#pragma once
#include "core/IShape.h"
#include "core/Linalg.h"
#include "core/IMaterial.h"

#include <memory>
#include <optional>

namespace shapes
{
    class Sphere final: public core::IShape
    {
    public:
        Sphere(const core::Vec3& pos, const core::Prec& radius, std::shared_ptr<const core::IMaterial> material);

        
        virtual std::optional<core::IntersectionData> Intersect(const core::Ray& ray) const final;
        virtual const core::SurfaceProperties GetSurfaceProperties(const core::Ray& ray,const core::IntersectionData& data) const final;

        virtual core::Bounds3D<core::Prec> WorldBoundary() const final;
        virtual std::vector<std::shared_ptr<const core::IMaterial>> GetMaterials() const final;
    
    private:
        const core::Vec3 _pos;
        const core::Prec _radius;
        const std::shared_ptr<const core::IMaterial> _material;
    };
}