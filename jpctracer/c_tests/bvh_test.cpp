#include "geometry.hpp"
#include <iostream>

#include "intersect_wrapper.hpp"
namespace jpc
{

jpc::sphere_mesh sphs_create(float* position_x, float* radii, uint n)
{
    jpc::sphere_mesh mesh;
    mesh.geometries.reserve(n);
    mesh.material_ids.reserve(n);
    for (uint i = 0; i < n; i++)
    {
        mesh.geometries.push_back(
            jpc::sphere_geometry{{position_x[i], 0, 0}, radii[i]});
        mesh.material_ids.push_back(0);
    }
    return mesh;
}

void simple_spheres()
{
    const int n = 6;
    float     pos[] = {0, 1, 2, 3, 4, 5};
    float     rad[] = {1, 1, 1, 1, 1, 1};

    scene_t                    scn;
    bvh_mesh<jpc::sphere_mesh> mesh(sphs_create(pos, rad, n));
    c::sphere_mesh_t           c_mesh = ctype(mesh);

    geom_log_write("created_bounds.obj");
    c::bvh_log(c_mesh.bvh_tree);
    geom_log_write("bvh sphs.obj");

    c::ray_t ray = {
        .origin = {7, ERROR_THICKNESS, ERROR_THICKNESS},
        .direction = {-1, ERROR_THICKNESS, ERROR_THICKNESS},
        .clip_end = 9,
    };

    c::hit_point_t hit;

    c::intersect_closest(
        ray, c_mesh.bvh_tree, c::spheres_intersect, &c_mesh, &hit);

    assert_near(hit.distance, 1);
    assert(hit.mesh_id == 5);

    std::cout << "Test Passed\n";
}

void simple_instances()
{

    const int n = 6;
    float     pos[] = {0, 1, 2, 3, 4, 5};
    float     rad[] = {1, 1, 1, 1, 1, 1};

    c::ray_t ray = {
        .origin = {13, ERROR_THICKNESS, ERROR_THICKNESS},
        .direction = {-1, ERROR_THICKNESS, ERROR_THICKNESS},
        .clip_end = 400,
    };

    scene_t        scn;
    auto           sps = scn.spheres.push_back(sphs_create(pos, rad, n));
    c::hit_point_t hit;

    float s = 2;

    instance_t inst;
    inst.mesh = sps;
    inst.transformation
        = std::array<float, 16>{s, 0, 0, 0, 0, s, 0, 0, 0, 0, s, 0, 0, 0, 0, 1};

    scn.instances.push_back(inst);
    c::scene_t c_scn = ctype(scn);
    
    
    bool did_inter = ray_intersect_c3(&c_scn.geometries,&ray,&hit);
    assert(did_inter);


    assert_near(hit.distance, 1);
    assert(hit.mesh_id == 5);


    assert_near(hit.location[0], 12);
    assert_near(hit.location[1], 0);
    assert_near(hit.location[2], 0);

    assert_near(hit.normal[0], 1);
    assert_near(hit.normal[1], 0);
    assert_near(hit.normal[2], 0);

    std::cout << "Instance test passed\n";
}
} // namespace jpc
int main()
{
    jpc::simple_spheres();
    jpc::simple_instances();
}
