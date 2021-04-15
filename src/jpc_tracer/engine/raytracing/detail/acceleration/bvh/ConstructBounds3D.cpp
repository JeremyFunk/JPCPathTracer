#include "ConstructBounds3D.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include <algorithm>



namespace jpctracer::raytracing
{
    std::vector<Bounds3D> GenerateTriangleBounds(const TriangleMesh& mesh)
    {
        std::vector<Bounds3D> bounds;
        bounds.reserve(mesh.TriangleGeometries.size());

        for(const auto& triangle : mesh.TriangleGeometries)
        {
            const Vec3 v1 = mesh.Vertices[triangle.Vertex1Id];
            const Vec3 v2 = mesh.Vertices[triangle.Vertex2Id];
            const Vec3 v3 = mesh.Vertices[triangle.Vertex3Id];

            const Vec3 max {std::max({v1[0], v2[0], v3[0]}), 
                            std::max({v1[1], v2[1], v3[1]}),
                            std::max({v1[2], v2[2], v3[2]})};

            const Vec3 min {std::min({v1[0], v2[0], v3[0]}), 
                            std::min({v1[1], v2[1], v3[1]}),
                            std::min({v1[2], v2[2], v3[2]})};

            bounds.emplace_back(min, max);
        }

        return bounds;
    }
    
    std::vector<Vec3> GetTriangleCenters(const TriangleMesh& mesh) 
    {
        std::vector<Vec3> center;
        center.reserve(mesh.TriangleGeometries.size());

        for(const auto& triangle : mesh.TriangleGeometries)
            center.emplace_back((1.0/3.0) * (mesh.Vertices[triangle.Vertex1Id] + mesh.Vertices[triangle.Vertex2Id] + mesh.Vertices[triangle.Vertex3Id]));

        return center;
    }
    
    std::vector<Bounds3D> GenerateSphereBounds(const SphereMesh& mesh)
    {
        std::vector<Bounds3D> bounds;
        bounds.reserve(mesh.Spheres.size());

        for(const auto& sphere : mesh.Spheres)
        {
            const Vec3 min {sphere.Position[0] - sphere.Radius, sphere.Position[1] - sphere.Radius, sphere.Position[2] - sphere.Radius};
            const Vec3 max {sphere.Position[0] + sphere.Radius, sphere.Position[1] + sphere.Radius, sphere.Position[2] + sphere.Radius};

            bounds.emplace_back(min, max);
        }

        return bounds;
    }
    
    std::vector<Vec3> GetSphereCenters(const SphereMesh& mesh)
    {
        std::vector<Vec3> center;
        center.reserve(mesh.Spheres.size());

        for(const auto& sphere : mesh.Spheres)
            center.emplace_back(sphere.Position);

        return center;
    }
    
    Vec3 GetBoxCenter(const Bounds3D& bound) 
    {
        return 0.5 * (bound.Min + bound.Max);
    }
}