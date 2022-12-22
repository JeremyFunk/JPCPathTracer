#pragma once
// this module is not thread safe!
#include "geometry.hpp"
#include <random>

namespace jpc
{
void          set_seed(std::uint32_t seed);
std::uint32_t   get_seed();
std::mt19937& get_random_engine();

template <class DistPos, class DistRad, class DistMat>
// DistPos, DistRad are distribution of type float
// DistMat is a distribution of type uint
sphere_mesh random_sphs(DistPos distribution_position,
                        DistRad distribution_radius,
                        DistMat distribution_mat,
                        uint    n)
{
    sphere_mesh mesh;
    mesh.geometries.reserve(n);
    mesh.material_ids.reserve(n);
    std::mt19937& engine = get_random_engine();
    while (n--)
    {
        float3 position;
        for (auto& p : position)
            p = distribution_position(engine);

        mesh.geometries.push_back(
            sphere_geometry{position, distribution_radius(engine)});

        mesh.material_ids.push_back(distribution_mat(engine));
    }
    return mesh;
}

inline sphere_mesh random_sphs_uniform(float position_min,
                                       float position_max,
                                       float radius_min,
                                       float radius_max,
                                       uint  materials_n,
                                       uint  n)
{
    return random_sphs(
        std::uniform_real_distribution<float>(position_min, position_max),
        std::uniform_real_distribution<float>(radius_min, radius_max),
        std::uniform_int_distribution<std::uint32_t>(0, materials_n-1),
        n);
}

template <typename DistS, typename DistP>
std::array<float, 16> random_transformation(DistS distribution_scale,
                                            DistP distribution_position)
{

    std::mt19937& e = get_random_engine();
    // clang-format off
    std::array<float,16> dst = {
        distribution_scale(e),   0,                       0,                       0,
        0,                       distribution_scale(e),   0,                       0,
        0,                       0,                       distribution_scale(e),   0,
        distribution_position(e),distribution_position(e),distribution_position(e),1,
    };
    // clang-format on
    return dst;
}

template <typename DistS, typename DistP, typename DistM>
std::vector<instance_t> random_instances(DistS distribution_scale,
                                         DistP distribution_position,
                                         DistM distribution_mesh,
                                         uint  n)
{

    std::mt19937&           e = get_random_engine();
    std::vector<instance_t> instances(n);
    for (instance_t& inst : instances)
    {
        inst.mesh = distribution_mesh(e);
        inst.transformation
            = random_transformation(distribution_scale, distribution_position);
    }
    return instances;
}

template <class T> class TD;
// I random access iterator
template <class I, class Dist> struct range_distribution
{
    using difference_type = typename std::iterator_traits<I>::difference_type;
    using value_type = typename std::iterator_traits<I>::value_type;
    I begin;

    Dist dist;

    template <typename T> value_type& operator()(T& engine)
    {
        // TD<decltype(begin)> begin_t;
        return *std::next(begin, difference_type(dist(engine)));
    }
};

template <class I>
auto make_uniform_range_distribution(
    I                                                 begin,
    typename std::iterator_traits<I>::difference_type n)
{
    using difference_type = typename std::iterator_traits<I>::difference_type;
    std::uniform_int_distribution<difference_type> dist(0, n-1);
    return range_distribution{begin, dist};
}

template <class T> auto make_uniform_distribution(handle_vector<T>& vec)
{
    return [dist = std::uniform_int_distribution<uint>(0, vec.count()-1),
            begin = vec.begin()](auto engine) mutable {
        uint i = dist(engine);
        auto it = begin + i;
        uint id = it.state().id;
        using handle_t = typename handle_vector<T>::handle_t;
        return handle_t(id, *it);
    };
}

struct random_scene_params
{
    float min_scale = 0.5;
    float max_scale = 2;
    float min_translation = -200;
    float max_translation = 200;
    float min_position = -2;
    float max_position = 2;
    float min_radius = 0.5;
    float max_radius = 2;
    uint  material_n = 20;
    uint  spheres_n = 30;
    uint  sphere_meshs_n = 20;
    uint  instances_n = 40;
};

inline scene_t random_scene(const random_scene_params& params)
{

    std::mt19937& e = get_random_engine();
    scene_t       scn;
    scn.spheres.reserve(params.sphere_meshs_n);
    for (uint i = 0; i < params.sphere_meshs_n; i++)
    {
        scn.spheres.push_back(random_sphs_uniform(params.min_position,
                                                  params.max_position,
                                                  params.min_radius,
                                                  params.max_radius,
                                                  params.material_n,
                                                  params.spheres_n));
    }
    auto sphs_distribution = make_uniform_distribution(scn.spheres);
    scn.instances
        = random_instances(std::uniform_real_distribution<float>(
                               params.min_scale, params.max_scale),
                           std::uniform_real_distribution<float>(
                               params.min_translation, params.max_translation),
                           sphs_distribution,
                           params.instances_n);

    scn.materials.reserve(params.material_n);
    auto sh_begin = scn.shaders.begin();
    auto sh_end = scn.shaders.end();
    auto sh_it = sh_begin;
    for (uint i = 0; i < params.material_n; i++)
    {
        if (sh_it == sh_end)
        {
            sh_it = sh_begin;
        }
        scn.materials.push_back(material(sh_it->second));
        sh_it++;
    }
    return scn;
}

}; // namespace jpc
