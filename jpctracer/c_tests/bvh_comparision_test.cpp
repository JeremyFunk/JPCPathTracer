#include "Timer.h"
#include <cassert>

extern "C"
{
#define GLM_FLT_EPSILON 1e-3
#include "bvh/bvh.h"
#include "jpc_api.h"
#include "types.h"

    bool ray_intersect(const geometries_t* geometries,
                       ray_t*              ray,
                       hit_point_t*        out_hitpoint);
    bool ray_intersect_c(const geometries_t* geometries,
                         ray_t*              ray,
                         hit_point_t*        out_hitpoint);
    bool ray_intersect_naive(const geometries_t* geometries,
                             ray_t*              ray,
                             hit_point_t*        out_hitpoint);

#include "cglm/cglm.h"
}

#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

bool ray_intersect_cpp(const geometries_t* geometries,
                       ray_t*              ray,
                       hit_point_t*        out_hitpoint);

typedef bool (*ray_intersect_f)(const geometries_t* geometries,
                                ray_t*              ray,
                                hit_point_t*        out_hitpoint);
double time_intersect_rays(ray_intersect_f     intersect,
                           hit_point_t*        hits,
                           const geometries_t* geoms,
                           ray_t*              rays,
                           int                 n)
{
    timer t;
    t.start();
    ray_t ray;
    for (int i = 0; i < n; i++)
    {
        ray = rays[i];
        intersect(geoms, &ray, hits + i);
    }
    return t.stop();
}

bool operator==(hit_point_t a, hit_point_t b)
{
    if (a.material_id == -1 && b.material_id == -1)
        return true;
    return glm_vec3_eqv_eps(a.location, b.location)
           //&& a.material_id == b.material_id
           //&& glm_vec3_eqv_eps(a.normal, b.normal)//changes when two overlap
           && glm_vec2_eqv_eps(a.uvs, b.uvs);
}

void print_hitp(hit_point_t hit)
{
    std::cout << "hit: \n";
    std::cout << "location:" << hit.location[0] << "," << hit.location[1] << ","
              << hit.location[2] << "\n";
    std::cout << "material_id: " << hit.material_id << "\n";
    std::cout << "normal: " << hit.normal[0] << "," << hit.normal[1] << ","
              << hit.normal[2] << "\n";
    std::cout << "uv: " << hit.uvs[0] << "," << hit.uvs[1] << "\n";
}

int main()
{

    // First create an instance of an engine.
    std::random_device rnd_device;
    // Specify the engine and distribution.
    std::uint32_t seed = rnd_device();
    // seed = 497162344;
    //  seed = 4159810338;
    seed = 659995227;
    std::mt19937 engine{seed}; // Generates random integers
    std::cout << "seed: " << seed << "\n";
    std::uniform_real_distribution<float> float_dist{-500, 500};

    std::uniform_real_distribution<float> float_dist_rad{0, 1. / 500.};

    auto fgen = [&]() { return float_dist(engine); };

    uint  ray_n = 100000;
    float clip_end = 400;
    uint  sphs_n = 10000;
    uint  sphs_meshes_n = 50;
    uint  instances_n = 100;
    uint  mat_slots_n = 2;
    uint  mat_slots_per_mesh_n = 1;
    assert(mat_slots_per_mesh_n < mat_slots_n);

    std::vector<std::uint32_t> mat_slots(mat_slots_n);
    std::vector<float>         sphs_radii(sphs_n);
    std::vector<float3>        sphs_positions(sphs_n);
    std::vector<std::uint32_t> sphs_material_slot_ids(sphs_n);
    std::vector<std::uint32_t> sphs_mesh_end_idx(sphs_meshes_n);
    std::vector<instance_t>    instances(instances_n);

    std::ranges::generate(sphs_radii, [&]() { return float_dist_rad(engine); });
    for (float3& pos : sphs_positions)
    {
        for (int i = 0; i < 3; i++)
            pos[i] = fgen();
    }

    std::uniform_int_distribution<std::uint32_t> uint_dist1{
        0, static_cast<unsigned int>(mat_slots_per_mesh_n - 1)};
    std::ranges::generate(sphs_material_slot_ids,
                          [&]() { return uint_dist1(engine); });

    std::uniform_int_distribution<std::uint32_t> uint_dist2{
        1, static_cast<unsigned int>(sphs_n)}; // sphs_n-1 ?
    std::ranges::generate(sphs_mesh_end_idx,
                          [&]() { return uint_dist2(engine); });
    std::ranges::sort(sphs_mesh_end_idx);
    auto mesh_end_idx_end
        = std::unique(sphs_mesh_end_idx.begin(), sphs_mesh_end_idx.end());
    sphs_meshes_n = std::distance(sphs_mesh_end_idx.begin(), mesh_end_idx_end);

    std::uniform_int_distribution<std::uint32_t> uint_mat_slots{
        0, static_cast<unsigned int>(mat_slots_n - 1)};
    std::ranges::generate(mat_slots, [&]() { return uint_mat_slots(engine); });

    std::uniform_int_distribution<std::uint32_t> uint_mat_slot_start{
        0, static_cast<unsigned int>(mat_slots_n - mat_slots_per_mesh_n - 1)};

    std::uniform_real_distribution<float> float_scale{-2, 2};

    std::uniform_int_distribution<std::uint32_t> uint_mesh_id{
        0, sphs_meshes_n - 1};
    for (instance_t& inst : instances)
    {
        inst.mesh_id = uint_mesh_id(engine);
        inst.material_slot_start = uint_mat_slot_start(engine);
        mat4 tmp;
        glm_mat4_identity(tmp);
        float scale = float_scale(engine);
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                inst.transformations[i][j] = tmp[i][j] * scale;
        inst.type = JPC_SPHERE;
    }

    geometries_t geoms = 
    {
        .instances_count = instances_n, .instances = instances.data(),
        .triangles = {
            .verticies_count=0,
            .normal_count=0,
            .uvs_count=0,
            .faces_count=0,
            .mesh_count=0,

        },
        .spheres
            = {.count = sphs_n,
               .positions = sphs_positions.data(),
               .radii = sphs_radii.data(),
               .material_slot_id = sphs_material_slot_ids.data(),
               .mesh_count = sphs_meshes_n,
               .mesh_end_idx = sphs_mesh_end_idx.data(),
        },
       
        .material_slots = mat_slots.data(),
        .material_slots_count = mat_slots_n,
    };

    geoms.bvhtree_spheres = bvhtree_build_spheres(geoms.spheres);
    geoms.bvhtree_triangles = bvhtree_build_triangles(geoms.triangles);
    geoms.bvhtree_instances = bvhtree_build_instances(&geoms);

    std::vector<ray_t> rays(ray_n);
    for (ray_t& ray : rays)
    {
        ray.clip_end = clip_end;
        for (int i = 0; i < 3; i++)
        {
            ray.direction[i] = float_dist(engine);
            ray.origin[i] = float_dist(engine);
        }
        glm_vec3_normalize(ray.direction);
    }

    hit_point_t default_hit{.location = {0, 0, 0, 0},
                            .material_id = -1,
                            .uvs = {0, 0},
                            .normal = {0, 0, 0}};

    std::vector<hit_point_t> hit_c1(ray_n, default_hit);
    std::vector<hit_point_t> hit_c2(ray_n, default_hit);
    std::vector<hit_point_t> hit_cpp(ray_n, default_hit);

    std::vector<hit_point_t> hit_naive(ray_n, default_hit);

    ray_intersect_f f = ray_intersect_c;
    double          t_c1 = time_intersect_rays(ray_intersect,
                                      hit_c1.data(),
                                      &geoms,
                                      rays.data(),
                                      ray_n); // ray_intersect
    double          t_c2 = time_intersect_rays(
        ray_intersect_c, hit_c2.data(), &geoms, rays.data(), ray_n);

    double t_cpp = time_intersect_rays(
        ray_intersect_cpp, hit_cpp.data(), &geoms, rays.data(), ray_n);

    double t_naive = time_intersect_rays(ray_intersect_naive,
                                         hit_naive.data(),
                                         &geoms,
                                         rays.data(),
                                         ray_n); // ray_intersect_naive

    /*
        double t_c1 = time_intersect_rays(f,
                                          hit_c1.data(),
                                          &geoms,
                                          rays.data(),
                                          ray_n); // ray_intersect
        double t_c2
            = time_intersect_rays(f, hit_c2.data(), &geoms, rays.data(), ray_n);

        double t_cpp
            = time_intersect_rays(f, hit_cpp.data(), &geoms, rays.data(),
    ray_n);

        double t_naive = time_intersect_rays(f,
                                             hit_naive.data(),
                                             &geoms,
                                             rays.data(),
                                             ray_n); // ray_intersect
    */

    vec3 tmp;
    for (int i = 0; i < ray_n; i++)
    {

        if (hit_naive[i] != hit_cpp[i] || hit_naive[i] != hit_c2[i]
            || hit_cpp[i] != hit_c2[i])
        {

            std::cout << "hit naive:\n";
            print_hitp(hit_naive[i]);

            glm_vec3_sub(rays[i].origin, hit_naive[i].location, tmp);
            std::cout << "Dist naive: " << glm_vec3_norm(tmp) << "\n";

            std::cout << "hit1: \n";
            print_hitp(hit_c1[i]);

            glm_vec3_sub(rays[i].origin, hit_c1[i].location, tmp);
            std::cout << "Dist1: " << glm_vec3_norm(tmp) << "\n";

            std::cout << "hit2: \n";
            print_hitp(hit_c2[i]);
            std::cout << "hitcpp: \n";
            print_hitp(hit_cpp[i]);

            glm_vec3_sub(rays[i].origin, hit_c2[i].location, tmp);
            std::cout << "Dist2: " << glm_vec3_norm(tmp) << "\n";

            glm_vec3_sub(rays[i].origin, hit_cpp[i].location, tmp);
            std::cout << "Dist_cpp: " << glm_vec3_norm(tmp) << "\n";
            std::cout << "i: " << i << "\n";
            printf("Assert!\n");
        }
        // assert(hit_c1[i] == hit_c2[i]);
        // assert(hit_c3[i] == hit_cpp[i]);
    }

    std::cout << "Ray count: " << ray_n << "\n";
    std::cout << "C1 : " << t_c1 << "\n";
    std::cout << "C2 : " << t_c2 << "\n";
    std::cout << "Cpp: " << t_cpp << "\n";

    std::cout << "C naive: " << t_naive << "\n";
}