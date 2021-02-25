#pragma once


#include "raytracing_core/include/Base.h"
#include "raytracing_core/include/Geometry.h"
#include <vector>
namespace jpc_rt_core
{

    struct TrianglesData
    {
        std::vector<Vec3> Positions;
        std::vector<Vec3> SupportVec1;
        std::vector<Vec3> SupportVec2;

        std::vector<Vec3> Normal1;
        std::vector<Vec3> Normal2;
        std::vector<Vec3> Normal3;

        std::vector<Vec2> UV1;
        std::vector<Vec2> UV2;
        std::vector<Vec2> UV3;

        std::vector<int> MaterialIDs;
        
    };

    struct SphereData
    {
        std::vector<Vec3> Positions;
        std::vector<Vec3> Radii;

        std::vector<int> MaterialIDs;
        
    };

    struct PrivateGeometry
    {
        TrianglesData Triangles;
        SphereData Spheres;
        StaticBVHType BvhType;
    };


}