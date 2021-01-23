#pragma once
#include "core/IShape.h"
#include "core/Linalg.h"
#include "core/IMaterial.h"

#include <memory>
#include <optional>

namespace jpc_tracer
{
    class Sphere final: public IShape
    {
    public:
        Sphere(const Vec3& pos, const Prec& radius, Ref<const IMaterial> material);

        
        virtual std::optional<IntersectionData> Intersect(const Ray& ray) const final;
        virtual const SurfaceProperties GetSurfaceProperties(const Ray& ray,const IntersectionData& data) const final;

        virtual Bounds3D<Prec> WorldBoundary() const final;
        virtual std::vector<Ref<const IMaterial>> GetMaterials() const final;
    
    private:
        const Vec3 _pos;
        const Prec _radius;
        const Ref<const IMaterial> _material;
    };
}