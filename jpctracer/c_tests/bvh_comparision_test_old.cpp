#include "Timer.h"
#include <cassert>
#include <iomanip>

extern "C"
{
#define GLM_FLT_EPSILON 1e-3
#include "bvh_old/bvh.h"
#include "jpc_api.h"
#include "types.h"

    bool ray_intersect(const geometries_t* geometries,
                       ray_t*              ray,
                       hit_point_t*        out_hitpoint);
    bool ray_intersect_c(const geometries_t* geometries,
                         ray_t*              ray,
                         hit_point_t*        out_hitpoint);

    bool ray_intersect_c2(const geometries_t* geometries,
                          ray_t*              ray,
                          hit_point_t*        out_hitpoint);
    bool ray_intersect_c3(const geometries_t* geometries,
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
           && glm_vec2_eqv_eps(a.uv, b.uv);
}

void print_hitp(hit_point_t hit)
{
    std::cout << "hit: \n";
    std::cout << "location:" << hit.location[0] << "," << hit.location[1] << ","
              << hit.location[2] << "\n";
    std::cout << "material_id: " << hit.material_id << "\n";
    std::cout << "normal: " << hit.normal[0] << "," << hit.normal[1] << ","
              << hit.normal[2] << "\n";
    std::cout << "uv: " << hit.uv[0] << "," << hit.uv[1] << "\n";
}

struct geometry_params
{

    uint sphs_n = 10000;
    uint sphs_meshes_n = 10;
    uint instances_n = 1000;
    uint mat_slots_n = 2;
    uint mat_slots_per_mesh_n = 1;
};

struct geometry_data
{

    geometry_data(const geometry_params& p)
        : mat_slots(p.mat_slots_n), sphs_radii(p.sphs_n),
          sphs_positions(p.sphs_n), sphs_material_slot_ids(p.sphs_n),
          sphs_mesh_end_idx(p.sphs_meshes_n), instances(p.instances_n),
          params(p)
    {
    }
    geometry_params                params;
    std::vector<std::uint32_t>     mat_slots;
    std::vector<sphere_geometry_t> sphs_geom;
    std::vector<std::uint32_t>     sphs_material_ids;
    std::vector<instance_t>        instances;

    geometries_t to_geometries()
    {

        return 
    {
        .instances_count = params.instances_n,
            .instances = instances.data(),
        .triangles = {
            .verticies_count=0,
            .normal_count=0,
            .uvs_count=0,
            .faces_count=0,
            .mesh_count=0,

        },
        .spheres
            = {.count = params.sphs_n,
               .positions = sphs_positions.data(),
               .radii = sphs_radii.data(),
               .material_slot_id = sphs_material_slot_ids.data(),
               .mesh_count = params.sphs_meshes_n,
               .mesh_end_idx = sphs_mesh_end_idx.data(),
        },
       
        .material_slots = mat_slots.data(),
        .material_slots_count = params.mat_slots_n,
    };
    }
};

geometry_data generate_geometry_data(geometry_params p)
{

    // First create an instance of an engine.
    std::random_device rnd_device;
    // Specify the engine and distribution.
    std::uint32_t seed = rnd_device();
    // seed = 497162344;
    //  seed = 4159810338;
    // seed = 659995227;
    std::mt19937 engine{seed}; // Generates random integers
    std::cout << "seed: " << seed << "\n";
    std::uniform_real_distribution<float> float_dist{-500, 500};

    std::uniform_real_distribution<float> float_dist_rad{0, 1};

    auto fgen = [&]() { return float_dist(engine); };

    geometry_data data(p);

    std::ranges::generate(data.sphs_radii,
                          [&]() { return float_dist_rad(engine); });
    for (float3& pos : data.sphs_positions)
    {
        for (int i = 0; i < 3; i++)
            pos[i] = fgen();
    }

    std::uniform_int_distribution<std::uint32_t> uint_dist1{
        0, static_cast<unsigned int>(p.mat_slots_per_mesh_n - 1)};
    std::ranges::generate(data.sphs_material_slot_ids,
                          [&]() { return uint_dist1(engine); });

    std::uniform_int_distribution<std::uint32_t> uint_dist2{
        1, static_cast<unsigned int>(p.sphs_n)}; // sphs_n-1 ?
    std::ranges::generate(data.sphs_mesh_end_idx, [&]() {
        uint i = uint_dist2(engine);
        assert(i != 0);
        return i;
    });
    std::ranges::sort(data.sphs_mesh_end_idx);
    auto mesh_end_idx_end = std::unique(data.sphs_mesh_end_idx.begin(),
                                        data.sphs_mesh_end_idx.end());
    p.sphs_meshes_n
        = std::distance(data.sphs_mesh_end_idx.begin(), mesh_end_idx_end);

    std::uniform_int_distribution<std::uint32_t> uint_mat_slots{
        0, static_cast<unsigned int>(p.mat_slots_n - 1)};
    std::ranges::generate(data.mat_slots,
                          [&]() { return uint_mat_slots(engine); });

    std::uniform_int_distribution<std::uint32_t> uint_mat_slot_start{
        0,
        static_cast<unsigned int>(p.mat_slots_n - p.mat_slots_per_mesh_n - 1)};

    std::uniform_real_distribution<float> float_scale{-2, 2};

    std::uniform_int_distribution<std::uint32_t> uint_mesh_id{
        0, p.sphs_meshes_n - 1};
    for (instance_t& inst : data.instances)
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
    data.params = p;
    return data;
}

std::vector<ray_t> generate_rays(uint ray_n, float clip_end)
{

    std::random_device rnd_device;
    std::uint32_t      seed = rnd_device();
    std::mt19937       engine{seed}; // Generates random integers
    std::uniform_real_distribution<float> float_dist{-500, 500};
    std::vector<ray_t>                    rays(ray_n);
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
    return rays;
}

struct bvh_builder
{
    void (*build)(geometries_t* geoms);
    void (*free)(geometries_t* geoms);
    bool operator==(const bvh_builder& b) const = default;
};

struct benchmark_case
{
    const char*     name;
    ray_intersect_f intersect;
    bvh_builder     builder;
};

void old_bvh_build(geometries_t* geoms)
{
    geoms->bvhtree_spheres = bvhtree_build_spheres(geoms->spheres);
    geoms->bvhtree_triangles = bvhtree_build_triangles(geoms->triangles);
    geoms->bvhtree_instances = bvhtree_build_instances(geoms);
}

void old_bvh_free(geometries_t* geoms)
{
    bvhtree_free(geoms->bvhtree_spheres);
    bvhtree_free(geoms->bvhtree_instances);
    bvhtree_free(geoms->bvhtree_triangles);
}

void new_bvh_build(geometries_t* geoms)
{
    bvhtree_spheres_build(geoms);
    bvhtree_triangles_build(geoms);
    bvhtree_instances_build(geoms);
}
void new_bvh_free(geometries_t* geoms)
{
    bvhtree_spheres_free(geoms);
    bvhtree_triangles_free(geoms);
    bvhtree_instances_free(geoms);
}

static bvh_builder old_bvh_builder = {
    .build = old_bvh_build,
    .free = old_bvh_free,
};

static bvh_builder new_bvh_builder = {
    .build = new_bvh_build,
    .free = new_bvh_build,
};

int main()
{

    uint            ray_n = 200;
    float           clip_end = 400;
    geometry_params p;
    geometry_data   data = generate_geometry_data(p);
    geometries_t    geoms = data.to_geometries();

    std::vector<ray_t> rays = generate_rays(ray_n, clip_end);

    hit_point_t                 default_hit{.location = {0, 0, 0, 0},
                            .material_id = -1,
                            .uv = {0, 0},
                            .normal = {0, 0, 0}};
    std::vector<benchmark_case> benchmarks = {
        //{"naive", ray_intersect_naive, old_bvh_builder},
        //{"old c1", ray_intersect, old_bvh_builder},
        //{"old c2 loop", ray_intersect_c, old_bvh_builder},
        //{"old c3 callback", ray_intersect_c2, old_bvh_builder},
        //{"old cpp", ray_intersect_cpp, old_bvh_builder},
        {"new c", ray_intersect_c3, new_bvh_builder},
    };

    std::vector<hit_point_t> hits(ray_n * benchmarks.size(), default_hit);
    std::vector<double>      times(benchmarks.size());

    bvh_builder last_builder = {.build = NULL, .free = NULL};
    for (int i = 0; i < benchmarks.size(); i++)
    {
        if (last_builder != benchmarks[i].builder)
        {
            if (last_builder.free != NULL)
            {
                last_builder.free(&geoms);
            }
            benchmarks[i].builder.build(&geoms);
        }
        hit_point_t* bench_hits = hits.data() + i * ray_n;
        times[i] = time_intersect_rays(benchmarks[i].intersect,
                                       bench_hits,
                                       &geoms,
                                       rays.data(),
                                       rays.size());
        last_builder = benchmarks[i].builder;
    }
    if (last_builder.free != NULL)
    {
        last_builder.free(&geoms);
    }
    for (int j = 0; j < ray_n; j++)
    {
        hit_point_t naive = hits[j];
        bool        already_false = false;
        for (int i = 1; i < benchmarks.size(); i++)
        {
            hit_point_t hit = hits[j + i * ray_n];
            if (hit != naive)
            {
                vec3 tmp;
                if (!already_false)
                {
                    std::cout << benchmarks[0].name << ":\n";
                    print_hitp(naive);
                    glm_vec3_sub(rays[i].origin, naive.location, tmp);
                    std::cout << "Dist: " << glm_vec3_norm(tmp) << "\n";
                    std::cout << "\n";
                    already_false = true;
                }

                std::cout << benchmarks[i].name << ":\n";
                print_hitp(hit);
                glm_vec3_sub(rays[i].origin, hit.location, tmp);
                std::cout << "Dist: " << glm_vec3_norm(tmp) << "\n";
                std::cout << "\n";
            }
        }
    }

    std::cout << "Ray count: " << ray_n << "\n";
    for (int i = 0; i < benchmarks.size(); i++)
    {
        std::cout << std::setw(20) << benchmarks[i].name << ": " << times[i]
                  << "\n";
    }
}
