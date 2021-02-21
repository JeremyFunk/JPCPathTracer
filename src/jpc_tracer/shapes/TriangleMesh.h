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
        
        TriangleMesh(Ref<std::vector<Prec>> vertices, Ref<std::vector<Prec>> normals, 
                        Ref<std::vector<Prec>> uvs, Ref<std::vector<int>> indices,
                        Ref<IMaterial> material, Ref<Transformation> transformation);
        
        void Transform(Ref<Transformation> data);

        Ref<std::vector<Prec>> Vertices;
        Ref<std::vector<Prec>> Normals;
        Ref<std::vector<Prec>> UVs;
        Ref<std::vector<int>> Indices;

        Ref<IMaterial> Material;
        Ref<Transformation> Transformation;

       Ref<std::vector<Ref<Triangle>>> GenerateTriangles();
    };

    class Triangle final: public IShape
    {
    public:
        Triangle() = default;
        Triangle(Ref<TriangleMesh> mesh, int index);

        virtual std::optional<IntersectionData> Intersect(const Ray& ray) const final;
        
        virtual const SurfaceProperties GetSurfaceProperties(const Ray& ray,const IntersectionData& data) const final;

        virtual Vec3 getCenter() const final;
        virtual Bounds3D<Prec> WorldBoundary() const final;


        virtual std::vector<Ref<const IMaterial>> GetMaterials() const final;

    private:
        Ref<TriangleMesh> _mesh;
        int _index;
    };
}