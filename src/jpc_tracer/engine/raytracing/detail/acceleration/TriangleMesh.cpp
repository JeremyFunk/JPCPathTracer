#include "TriangleMesh.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/raytracing/Base.h"

namespace jpctracer::raytracing
{
/*
p1 = position
support_vec_1 = p2-p1
support_vec_2 = p3-p1
*/
std::optional<IntersectionInfo> TriangleIntersect(const Ray& ray, Vec3 position, Vec3 support_vec_1, Vec3 support_vec_2)
{
    // JPC_LOG_INFO("RayDir {}, RayOrigin{}, Pos {}, Sup1 {}, Sup2{}", ray.Direction.to_string(),
    // ray.Origin.to_string(),
    //             position.to_string(), support_vec_1.to_string(), support_vec_2.to_string());
    // defines close to 0
    Prec epsilon = 0.00001;

    Vec3 point_direction = ray.Direction.cross(support_vec_2);
    Prec determinante = support_vec_1.dot(point_direction);

    // check backfacing or never intersection
    if (determinante < epsilon)
        return std::nullopt;

    Prec inv_determinante = 1 / determinante;

    Vec3 diff_origin_position = ray.Origin - position;

    // UV
    Prec u = diff_origin_position.dot(point_direction) * inv_determinante;

    Vec3 cross_op_s1 = diff_origin_position.cross(support_vec_1);
    Prec v = ray.Direction.dot(cross_op_s1) * inv_determinante;

    if (u < ERROR_THICCNESS || u > 1 - ERROR_THICCNESS || v < ERROR_THICCNESS || u + v > 1 - ERROR_THICCNESS)
        return std::nullopt;

    // intersection point
    Prec intersection_point_distance = support_vec_2.dot(cross_op_s1) * inv_determinante;

    if (intersection_point_distance < 0)
        return std::nullopt;

    // output
    IntersectionInfo output;
    output.Distance = intersection_point_distance;
    output.UV = {u, v};

    return std::make_optional(output);
}

Vec3 Interpolate(Vec3 v1, Vec3 v2, Vec3 v3, Prec u, Prec v)
{
    return u * v2 + v * v3 + (1 - u - v) * v1;
}
Vec2 Interpolate(Vec2 v1, Vec2 v2, Vec2 v3, Prec u, Prec v)
{
    return u * v2 + v * v3 + (1 - u - v) * v1;
}

SurfaceInteraction TriangleGetInteraction(const Ray& ray, IntersectionInfo info, int material_id, Vec3 normal_1,
                                          Vec3 normal_2, Vec3 normal_3, Vec2 uv_1, Vec2 uv_2, Vec2 uv_3)
{

    Vec3 point = Vec3(ray.Direction) * info.Distance + ray.Origin;
    Prec distance = info.Distance;

    Vec2 uv = Interpolate(uv_1, uv_2, uv_3, info.UV[0], info.UV[1]);
    Vec3 normal = Interpolate(normal_1, normal_2, normal_3, info.UV[0], info.UV[1]);

    SurfaceInteraction interaction = {point, normal.normalized(), uv, distance, material_id};
    return interaction;
}

std::optional<SurfaceInteraction> Intersect(const TriangleMesh& mesh, int idx, const Ray& ray,
                                            const int* materials_per_slot)
{
    auto [v1_id, v2_id, v3_id] = mesh.TriangleGeometries[idx];

    Vec3 pos = mesh.Vertices[v1_id];
    Vec3 sup_1 = mesh.Vertices[v2_id] - pos;
    Vec3 sup_2 = mesh.Vertices[v3_id] - pos;

    auto intersection = TriangleIntersect(ray, pos, sup_1, sup_2);

    if (intersection)
    {
        auto [norm_ids, uv_ids, slot_id] = mesh.TriangleShadings[idx];
        Vec2 uv_1 = mesh.UVs[uv_ids[0]];
        Vec2 uv_2 = mesh.UVs[uv_ids[1]];
        Vec2 uv_3 = mesh.UVs[uv_ids[2]];

        Vec3 norm_1 = mesh.Normals[norm_ids[0]];
        Vec3 norm_2 = mesh.Normals[norm_ids[1]];
        Vec3 norm_3 = mesh.Normals[norm_ids[2]];

        int material_id = *(slot_id + materials_per_slot);

        return TriangleGetInteraction(ray, *intersection, material_id, norm_1, norm_2, norm_3, uv_1, uv_2, uv_3);
    }

    return std::nullopt;
}

size_t GetSize(const TriangleMesh& mesh)
{
    return mesh.TriangleShadings.size();
}

Bounds3D BoundingBox(const TriangleMesh& mesh, int idx)
{
    auto [v1_id, v2_id, v3_id] = mesh.TriangleGeometries[idx];
    Vec3 v1 = mesh.Vertices[v1_id];
    Vec3 v2 = mesh.Vertices[v2_id];
    Vec3 v3 = mesh.Vertices[v3_id];

    Vec3 max_b = {std::max(v1[0], std::max(v2[0], v3[0])), std::max(v1[1], std::max(v2[1], v3[1])),
                  std::max(v1[2], std::max(v2[2], v3[2]))};

    Vec3 min_b = {std::min(v1[0], std::min(v2[0], v3[0])), std::min(v1[1], std::min(v2[1], v3[1])),
                  std::min(v1[2], std::min(v2[2], v3[2]))};

    return {min_b, max_b};
}

Vec3 GetCenter(const TriangleMesh& mesh, int idx)
{
    auto [v1_id, v2_id, v3_id] = mesh.TriangleGeometries[idx];
    Vec3 v1 = mesh.Vertices[v1_id];
    Vec3 v2 = mesh.Vertices[v2_id];
    Vec3 v3 = mesh.Vertices[v3_id];

    return {(v1[0] + v2[0] + v3[0]) / 3.f, (v1[1] + v2[1] + v3[1]) / 3.f, (v1[2] + v2[2] + v3[2]) / 3.f};
}

} // namespace jpctracer::raytracing