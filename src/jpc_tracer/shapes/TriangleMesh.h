#pragma once
#include "core/IMaterial.h"
#include "core/IShape.h"
#include "TriangleMesh.h"
#include "core/Bounds3D.h"
#include "core/Linalg.h"
#include "core/Transformation.h"
#include "shapes/TriangleMesh.h"
#include <memory>


namespace jpc_tracer
{
    class TriangleMesh;
    class Triangle;

    class TriangleMesh
    {
    public:
        TriangleMesh() = default;
        
        TriangleMesh(std::shared_ptr<std::vector<Prec>> vertices, std::shared_ptr<std::vector<Prec>> normals, 
                        std::shared_ptr<std::vector<Prec>> uvs, std::shared_ptr<std::vector<int>> indices,
                        std::shared_ptr<IMaterial> material, std::shared_ptr<Transformation> transformation);
        
        void Transform(std::shared_ptr<Transformation> data);

        std::shared_ptr<std::vector<Prec>> Vertices;
        std::shared_ptr<std::vector<Prec>> Normals;
        std::shared_ptr<std::vector<Prec>> UVs;
        std::shared_ptr<std::vector<int>> Indices;

        std::shared_ptr<IMaterial> Material;
        std::shared_ptr<Transformation> Transformation;

       std::shared_ptr<std::vector<std::shared_ptr<Triangle>>> GenerateTriangles();
    };

    class Triangle final: public IShape
    {
    public:
        Triangle() = default;
        Triangle(std::shared_ptr<TriangleMesh> mesh, int index);

        virtual std::optional<IntersectionData> Intersect(const Ray& ray) const final;
        
        virtual const SurfaceProperties GetSurfaceProperties(const Ray& ray,const IntersectionData& data) const final;

        virtual Bounds3D<Prec> WorldBoundary() const final;


        virtual std::vector<std::shared_ptr<const IMaterial>> GetMaterials() const final;

    private:
        std::shared_ptr<TriangleMesh> _mesh;
        int _index;
    };
}