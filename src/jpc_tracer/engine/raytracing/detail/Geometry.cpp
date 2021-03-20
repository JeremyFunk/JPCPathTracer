#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "geometry/PrivateRootGeometry.h"
#include "geometry/PrivateGeometry.h"
namespace jpctracer::raytracing {


    int Geometry::AddTriangle(Triangle triangle, int material_id) 
    {
        Vec3 vert_1 = triangle.Vert1.Position;
        Vec3 vert_2 = triangle.Vert2.Position;
        Vec3 vert_3 = triangle.Vert3.Position;

        Vec3 normal_1 = triangle.Vert1.Normal;
        Vec3 normal_2 = triangle.Vert2.Normal;
        Vec3 normal_3 = triangle.Vert3.Normal;

        Vec2 uv_1 = triangle.Vert1.UV;
        Vec2 uv_2 = triangle.Vert2.UV;
        Vec2 uv_3 = triangle.Vert3.UV;

        data->Triangles.Positions.push_back(vert_1);
        data->Triangles.SupportVec1.push_back(vert_2 - vert_1);
        data->Triangles.SupportVec2.push_back(vert_3 - vert_1);
        data->Triangles.Normal1.push_back(normal_1);
        data->Triangles.Normal2.push_back(normal_2);
        data->Triangles.Normal3.push_back(normal_3);
        data->Triangles.MaterialIDs.push_back(material_id);
        data->Triangles.UV1.push_back(uv_1);
        data->Triangles.UV2.push_back(uv_2);
        data->Triangles.UV3.push_back(uv_3);

        return data->Triangles.Positions.size()-1;
                
    }


    int Geometry::AddSphere(Sphere sphere, int material_id) 
    {
        data->Spheres.Positions.push_back(sphere.Position);
        data->Spheres.Radii.push_back(sphere.Radius);
        return data->Spheres.Radii.size()-1;
    }

    Geometry* RootGeometry::CreateGeometryInstance(Transformation transformation) 
    {
        data->StaticGeometries.push_back(PrivateGeometry{});
        PrivateGeometry* geometry = &data->StaticGeometries[data->StaticGeometries.size()-1];
        Geometry* result = new Geometry{geometry};
        return result;
    }

    RootGeometry::~RootGeometry() 
    {
        free(data);
    }







}