#pragma once
#include "IntersectionInfo.h"
#include "raytracing_core/include/SurfaceInteraction.h"
#include <optional>


namespace jpc_rt_core
{
    std::optional<IntersectionInfo> TriangleIntersect(const Ray& ray, Vec3 position, Vec3 support_vec_1,Vec3 support_vec_2);

    SurfaceInteraction TriangleGetInteraction(const Ray& ray, IntersectionInfo info, int material_id, Vec3 normal_1,Vec3 normal_2,Vec3 normal_3,Vec2 uv_1,Vec2 uv_2,Vec2 uv_3);
}