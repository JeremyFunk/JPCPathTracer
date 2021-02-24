#pragma once
#include "Base.h"

namespace jpc_rt_core {
    enum class InstanceBVHType
    {

    };

    enum class DynamicBVHType
    {

    };

    enum class StaticBVHType
    {

    };


    struct Geometry
    {
        Geometry(DynamicBVHType);
        void AddTriangle(Vec3 vert_1,Vec3 vert_2,Vec3 vert_3, Vec3 normal_1,Vec3 normal_2,Vec3 normal_3);
        void AddSphere(Vec3 position, Vec3 radius);
        ~Geometry();
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

    

    struct RootGeometry
    {
        RootGeometry(InstanceBVHType instance_bvh, DynamicBVHType dynamic_bvh, StaticBVHType static_bvh);
        Geometry* CreateGeometryInstance(Transformation transforamtion);

        Geometry* CreateAnimatedGeometryInstance(Transformation(*transforamtion_getter)(AnimationData data, float time), 
        Bounds3D(*fit_bounding_box)(AnimationData data, Bounds3D, float min_time, float max_time) );
        ~RootGeometry();
    };


}