#pragma once
#include "Base.h"

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
        void AddTriangle(
            Vec3 vert_1,Vec3 vert_2,Vec3 vert_3, 
            Vec3 normal_1,Vec3 normal_2,Vec3 normal_3, 
            Vec2 uv_1,Vec2 uv_2,Vec2 uv_3 , 
            int material_id);
        void AddSphere(Vec3 position, Vec3 radius, int material_id);
        PrivateGeometry* data;
    };

    struct Transformation
    {
        Mat3x3 RotationScale;
        Vec3 Translation;
    };

    struct Bounds3D
    {
        Vec3 Min;
        Vec3 Max;
    };

    
    struct PrivateRootGeometry;

    using TransformationGetter = Transformation(*)(AnimationData data, float time);

    using FitBoundingBox = Bounds3D(*)(AnimationData data, Bounds3D, float min_time, float max_time);

    struct RootGeometry
    {
        Geometry* CreateGeometryInstance(Transformation transformation);

        Geometry* CreateAnimatedGeometryInstance(TransformationGetter transformation_getter, FitBoundingBox fit_bounding_box);
        ~RootGeometry();

        PrivateRootGeometry* data;
    };



    
}