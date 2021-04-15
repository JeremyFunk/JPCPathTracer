#pragma once
#include "Base.h"
#include "jpc_tracer/core/core.h"
#include <memory>
#include <vector>

namespace jpctracer::raytracing {

    enum class DynamicBVHType
    {
        NAIVE
    };

    enum class StaticBVHType
    {
        NAIVE
    };

    struct EntityId
    {
        const int Id;
    };

    enum class MeshTypes
    {
        TRIANGLE,
        SPHERE
    };

    struct MeshId 
    {
        MeshTypes type;
        size_t id;
    };


    struct TriangleGeometry
    {
        uint Vertex1Id;
        uint Vertex2Id;
        uint Vertex3Id;
    };

    struct TriangleShading
    {
        UInt3 NormalIds;
        UInt3 UVIds;
        uint MaterialSlot;
        
    };
    
    struct TriangleMesh
    {
        std::vector<Vec3> Vertices;
        std::vector<Vec3> Normals;
        std::vector<Vec2> UVs;
        
        std::vector<TriangleGeometry> TriangleGeometries;   
        std::vector<TriangleShading> TriangleShadings;     

    };

    struct Sphere
    {
        Vec3 Position;
        Prec Radius;
    };

    struct SphereMesh{
        std::vector<Sphere> Spheres;
        std::vector<int> MaterialSlots;    
    };

    

    
    
}