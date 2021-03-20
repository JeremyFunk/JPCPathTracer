#pragma once
#include "Base.h"
#include "jpc_tracer/core/core.h"
#include <memory>

namespace jpctracer::raytracing {
    enum class InstanceBVHType
    {
        NAIVE
    };

    enum class DynamicBVHType
    {
        NAIVE
    };

    enum class StaticBVHType
    {
        NAIVE
    };

    
    struct PrivateGeometry;

    struct Geometry
    {
        //int = Triangle id
        int AddTriangle(Triangle triangle, int material_id);
        //int = Sphere id
        int AddSphere(Sphere sphere, int material_id);
        PrivateGeometry* data;

    };


    
    struct PrivateRootGeometry;

    struct RootGeometry
    {
        Geometry* CreateGeometryInstance(Transformation transformation);

        Geometry* CreateAnimatedGeometryInstance(std::shared_ptr<IAnimated<Transformation>>);
        ~RootGeometry();

        PrivateRootGeometry* data;
        //Gets the Triangle in world Space
        Triangle GetTriangle(int id);
        Sphere GetSphere(int id);
    };



    
}