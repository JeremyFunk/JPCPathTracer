#include "jpc_tracer/raytracing/Geometry.h"
#include "geometry/PrivateRootGeometry.h"
#include "geometry/PrivateGeometry.h"
namespace jpctracer::raytracing {


    void Geometry::AddTriangle(Vec3 vert_1,Vec3 vert_2,Vec3 vert_3, 
        Vec3 normal_1,Vec3 normal_2,Vec3 normal_3, 
        Vec2 uv_1,Vec2 uv_2,Vec2 uv_3 , 
        int material_id) 
    {
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
                
    }


    void Geometry::AddSphere(Vec3 position, Vec3 radius, int material_id) 
    {
        data->Spheres.Positions.push_back(position);
        data->Spheres.Radii.push_back(radius);
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