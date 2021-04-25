#include "SphereMesh.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include <optional>

namespace jpctracer::raytracing
{
std::optional<SurfaceInteraction> IntersectOld(const SphereMesh& mesh, int idx, const Ray& ray,
                                               const int* materials_per_slot)
{
    if (ray.Origin[1] < -0.5 && ray.Origin[2] < 0)
    {
        int a = 0;
        JPC_LOG_INFO("test");
    }
    auto [sp_center, sp_radius] = mesh.Spheres[idx];
    Vec3 perfect_dir = ray.Origin - sp_center;

    Prec cos_ray_perfect = ray.Direction.dot(perfect_dir);

    // check how many solutions exist
    Prec checker = std::pow(cos_ray_perfect, 2);

    checker -= std::pow(perfect_dir.norm(), 2) - std::pow(sp_radius, 2);

    if (checker < 0) // => no solution
        return std::nullopt;

    // calculate solutions

    // min 1 solution
    Prec distance = -cos_ray_perfect;

    checker = std::sqrt(checker);

    Prec distance_temp = distance - checker;

    distance += checker;

    // choose smallest solution
    if (distance > distance_temp)
        distance = distance_temp;

    distance += ERROR_THICCNESS;

    // ray points in other direction
    if (distance <= 0)
        return std::nullopt;

    // output
    SurfaceInteraction interaction;

    interaction.Point = Vec3(ray.Direction) * distance + ray.Origin;
    interaction.Normal = (interaction.Point - sp_center).normalized();
    interaction.Distance = distance;
    interaction.UV = {0, 0};
    interaction.MaterialId = materials_per_slot[mesh.MaterialSlots[idx]];

    return interaction;
}

bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
    float discr = b * b - 4 * a * c;
    if (discr < 0)
        return false;
    else if (discr == 0)
        x0 = x1 = -0.5 * b / a;
    else
    {
        float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1)
        std::swap(x0, x1);

    return true;
}
// refrence
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
std::optional<SurfaceInteraction> Intersect(const SphereMesh& mesh, int idx, const Ray& ray,
                                            const int* materials_per_slot)
{
    Vec3 dir = ray.Direction;
    Vec3 orig = ray.Origin;
    auto [center, radius] = mesh.Spheres[idx];

    Prec radius2 = radius * radius;

    float t0, t1; // solutions for t if the ray intersects
#if 0 
    // geometric solution
    Vec3f L = center - orig; 
    float tca = L.dot(dir); 
    // if (tca < 0) return false;
    float d2 = L.dot(L) - tca * tca; 
    if (d2 > radius2) return false; 
    float thc = sqrt(radius2 - d2); 
    t0 = tca - thc; 
    t1 = tca + thc;
#else
    // analytic solution
    Vec3 L = orig - center;
    float a = dir.dot(dir);
    float b = 2 * dir.dot(L);
    float c = L.dot(L) - radius2;
    if (!solveQuadratic(a, b, c, t0, t1))
        return std::nullopt;
#endif
    if (t0 > t1)
        std::swap(t0, t1);

    if (t0 < 0)
    {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0)
            return std::nullopt; // both t0 and t1 are negative
    }

    Prec t = t0 - ERROR_THICCNESS;

    SurfaceInteraction interaction;

    interaction.Point = Vec3(ray.Direction) * t + ray.Origin;
    interaction.Normal = (interaction.Point - center).normalized();
    interaction.Distance = t;
    interaction.UV = {0, 0};
    interaction.MaterialId = materials_per_slot[mesh.MaterialSlots[idx]];

    return interaction;
}

size_t GetSize(const SphereMesh& mesh)
{
    return mesh.Spheres.size();
}
} // namespace jpctracer::raytracing