#include "SphereMesh.h"

namespace jpctracer::raytracing
{
std::optional<SurfaceInteraction> Intersect(const SphereMesh& mesh, int idx, const Ray& ray,
                                            const int* materials_per_slot)
{
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

size_t GetSize(const SphereMesh& mesh)
{
    return mesh.Spheres.size();
}
} // namespace jpctracer::raytracing