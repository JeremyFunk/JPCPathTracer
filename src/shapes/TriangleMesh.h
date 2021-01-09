#pragma once
#include "core/IMaterial.h"
#include "core/IShape.h"
#include "TriangleMesh.h"
#include "core/Bounds3D.h"
#include "core/Linalg.h"
#include "core/Transformation.h"
#include "shapes/TriangleMesh.h"
#include <memory>


namespace shapes
{
    class TriangleMesh;
    class Triangle;

    class TriangleMesh
    {
    public:
        TriangleMesh() = default;
        
        TriangleMesh(std::shared_ptr<std::vector<core::Prec>> vertices, std::shared_ptr<std::vector<core::Prec>> normals, 
                        std::shared_ptr<std::vector<core::Prec>> uvs, std::shared_ptr<std::vector<int>> indices,
                        std::shared_ptr<core::IMaterial> material, std::shared_ptr<core::Transformation> transformation);
        
        void Transform(std::shared_ptr<core::Transformation> data);

        std::shared_ptr<std::vector<core::Prec>> Vertices;
        std::shared_ptr<std::vector<core::Prec>> Normals;
        std::shared_ptr<std::vector<core::Prec>> UVs;
        std::shared_ptr<std::vector<int>> Indices;

        std::shared_ptr<core::IMaterial> Material;
        std::shared_ptr<core::Transformation> Transformation;

       std::shared_ptr<std::vector<std::shared_ptr<Triangle>>> GenerateTriangles();
    };

    class Triangle final: public core::IShape
    {
    public:
        Triangle() = default;
        Triangle(std::shared_ptr<TriangleMesh> mesh, int index);

        virtual std::optional<core::IntersectionData> Intersect(const core::Ray& ray) const final;
        
        virtual const core::SurfaceProperties GetSurfaceProperties(const core::Ray& ray,const core::IntersectionData& data) const final;

        virtual core::Bounds3D<core::Prec> WorldBoundary() const final;

    private:
        std::shared_ptr<TriangleMesh> _mesh;
        int _index;
    };
}