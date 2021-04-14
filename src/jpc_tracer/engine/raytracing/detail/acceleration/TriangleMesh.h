#pragma once
#include "IntersectionInfo.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include <optional>
#include "jpc_tracer/engine/raytracing/Geometry.h"


namespace jpctracer::raytracing
{
    std::optional<IntersectionInfo> TriangleIntersect(const Ray& ray, Vec3 position, Vec3 support_vec_1,Vec3 support_vec_2) ;
    SurfaceInteraction TriangleGetInteraction(const Ray& ray, IntersectionInfo info,int material_id, Vec3 normal_1,Vec3 normal_2,Vec3 normal_3,Vec2 uv_1,Vec2 uv_2,Vec2 uv_3);

    std::optional<SurfaceInteraction> Intersect(const TriangleMesh& mesh, int idx, const Ray& ray, const int* materials_per_slot);

    size_t GetSize(const TriangleMesh& mesh);

    Bounds3D BoundingBox(const TriangleMesh& mesh, int idx);

    Vec3 GetCenter(const TriangleMesh& mesh, int idx);

}