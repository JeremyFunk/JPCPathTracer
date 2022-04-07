#include "Timer.h"

#define GLM_FLT_EPSILON 1e-4
#include "geometry.hpp"
#include "intersect_wrapper.hpp"
#include "random_geometry.hpp"
#include <iostream>
#include <random>

namespace jpc
{

template <typename DistO, typename DistD>
std::vector<c::ray_t> random_rays(DistO distribution_origin,
                                  DistD distribution_direction,
                                  float clip_end,
                                  uint  n)
{
    auto e = get_random_engine();

    std::vector<c::ray_t> rays(n);
    for (c::ray_t& ray : rays)
    {
        for (int i = 0; i < 3; i++)
            ray.direction[i] = distribution_direction(e);
        glm_vec3_normalize(ray.direction);
        for (int i = 0; i < 3; i++)
            ray.origin[i] = distribution_origin(e);

        ray.clip_end = clip_end;
    }

    return rays;
}

std::vector<c::ray_t> random_rays_uniform(float min_origin,
                                          float max_origin,
                                          float clip_end,
                                          uint  n)
{
    return random_rays(
        std::uniform_real_distribution<float>(min_origin, max_origin),
        std::uniform_real_distribution<float>(-1, 1),
        clip_end,
        n);
}

typedef bool (*ray_intersect_f)(const c::geometries_t* geometries,
                                const c::ray_t*        ray,
                                c::hit_point_t*        out_hitpoint);

bool operator==(c::ray_t a, c::ray_t b)
{
    return glm_vec3_eqv_eps(a.direction, b.direction)
           && glm_vec3_eqv_eps(a.origin, b.origin)

           && glm_eq(a.clip_end, b.clip_end);
}

bool operator==(const c::hit_point_t& a, const c::hit_point_t& b)
{
    if (a.material_id == -1 && b.material_id == -1)
        return true;
    return glm_vec3_eqv_eps((float*)a.location, (float*)b.location)
           //&& a.material_id == b.material_id
           //&& glm_vec3_eqv_eps(a.normal, b.normal)//changes when two overlap
           && glm_vec2_eqv_eps((float*)a.uv, (float*)b.uv);
}
double time_intersect_rays(ray_intersect_f        intersect,
                           c::hit_point_t*        hits,
                           const c::geometries_t* geoms,
                           c::ray_t*              rays,
                           int                    n)
{
    timer t;
    t.start();
    c::ray_t ray;
    for (int i = 0; i < n; i++)
    //for(int i = 7;i<8;i++)
    {
        ray = rays[i];
        intersect(geoms, &ray, hits + i);
        assert(ray == rays[i]);
    }
    return t.stop();
}

void print_vec3(vec3 v)
{
    std::cout << "(" << v[0] << "," << v[1] << "," << v[2] << ")\n";
}

void print_vec2(vec2 v)
{
    std::cout << "(" << v[0] << "," << v[1] << ")\n";
}

void print_ray(c::ray_t ray)
{
    std::cout << "\torg: ";
    print_vec3(ray.origin);
    std::cout << "\n\tdir: ";
    print_vec3(ray.direction);
    std::cout << "\n\tclip_end: " << ray.clip_end << "\n";
}

void print_hitpoint(c::hit_point_t hit, c::ray_t ray)
{
    if (hit.material_id == -1)
    {
        std::cout << "\tNo hit\n";
        return;
    }
    std::cout << "\thit_distance:\t" << hit.distance;

    vec3 loc_origin_diff;
    glm_vec3_sub(hit.location, ray.origin, loc_origin_diff);
    float dist = glm_vec3_norm(loc_origin_diff);

    std::cout << "\n\tray_distance:\t" << dist;
    glm_vec3_normalize(loc_origin_diff);
    std::cout << "\n\thit_p direction:\t";
    print_vec3(loc_origin_diff);
    std::cout << "\n\tinstance_id:\t" << hit.instance_id;
    std::cout << "\n\tlocation:\t";
    print_vec3(hit.location);
    std::cout << "\n\tmaterial_id:\t" << hit.material_id;
    std::cout << "\n\tmesh_id:\t" << hit.mesh_id;
    std::cout << "\n\tnormal:\t";
    print_vec3(hit.normal);
    std::cout << "\n\tuv:\t";
    print_vec2(hit.uv);
    std::cout << "\n";
}

void print_diagonstics(c::ray_t       ray,
                       c::hit_point_t hit_a,
                       std::string    name_a,
                       c::hit_point_t hit_b,
                       std::string    name_b)
{
    std::cout << "-----------------\n";
    std::cout << "ray:\n";
    print_ray(ray);
    std::cout << "hit: " << name_a << "\n";
    print_hitpoint(hit_a, ray);
    std::cout << "hit: " << name_b << "\n";
    print_hitpoint(hit_b, ray);
}

static const c::hit_point_t default_hit = {.material_id = -1};
void run_diagonstics(const std::vector<std::vector<c::hit_point_t>>& hits,
                     const std::vector<std::string>&                 names,
                     const std::vector<c::ray_t>&                    rays)
{

    assert(names.size() == hits.size());
    if (hits.size() == 0)
    {
        std::cout << "no hits\n";
        return;
    }

    assert(hits[0].size() == rays.size());
    uint intersected_n
        = std::count_if(hits[0].cbegin(),
                     hits[0].cend(),
                     [](c::hit_point_t a) { return a != default_hit; });

    std::cout << "intersections count: " << intersected_n << "\n";

    for (uint ray_i = 0; ray_i < rays.size(); ray_i++)
    {
        const c::hit_point_t& first_hit = hits[0][ray_i];
        const c::ray_t&       ray = rays[ray_i];
        const std::string&    first_name = names[0];

        for (uint method_i = 1; method_i < names.size(); method_i++)
        {
            const c::hit_point_t& second_hit = hits[method_i][ray_i];
            const std::string&    second_name = names[method_i];
            if (second_hit != first_hit)
            {

                std::cout<<"Error with ray: "<<ray_i<<"\n";
                print_diagonstics(
                    ray, first_hit, first_name, second_hit, second_name);
            }
        }
    }
}

void compare_methodes(std::vector<ray_intersect_f> methods,
                      std::vector<std::string>     method_names,
                      uint                         ray_n,
                      random_scene_params          params)
{
    assert(methods.size() == method_names.size());
    std::cout << "seed:" << get_seed() << "\n";

    scene_t    scn = random_scene(params);
    c::scene_t c_scn = ctype(scn);

    std::vector<std::vector<c::hit_point_t>> hits(
        methods.size(), std::vector<c::hit_point_t>(ray_n, default_hit));

    std::vector<c::ray_t> rays = random_rays_uniform(
        params.min_position, params.max_position, 400, ray_n);

    std::vector<double> times(methods.size(), -1);

    for (uint method_i = 0; method_i < methods.size(); method_i++)
    {
        times[method_i] = time_intersect_rays(methods[method_i],
                                              hits[method_i].data(),
                                              &c_scn.geometries,
                                              rays.data(),
                                              ray_n);
    }

    run_diagonstics(hits, method_names, rays);

    for (uint method_i = 0; method_i < methods.size(); method_i++)
    {
        std::cout << method_names[method_i] << " time: " << times[method_i]
                  << "\n";
    }
}

} // namespace jpc

int main()
{
    //jpc::set_seed( 1824929880);
    jpc::compare_methodes(
        {/*jpc::c::ray_intersect_naive,*/ jpc::c::ray_intersect_c3},
        {/*"naive",*/ "bvh"},
        1e6,
        jpc::random_scene_params{
            .min_radius=1,
            .max_radius=5,
            .spheres_n=100,
            .sphere_meshs_n=100,
            .instances_n = 200,
        });
}
