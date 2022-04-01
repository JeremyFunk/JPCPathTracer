#include "geometry.hpp"
/*
extern "C"
{
#include "jpc_api.h"
}*/
namespace jpc
{
c::sphere_mesh_t get_ctype(jpc::sphere_mesh& m, c::bvh_tree_t* tree = nullptr)
{
    assert(m.geometries.size() == m.material_ids.size());
    return {
        .count = static_cast<uint>(m.geometries.size()),
        .geometries = (c::sphere_geometry_t*)m.geometries.data(),
        .material_ids = m.material_ids.data(),
        .bvh_tree = tree,
    };
}
c::triangle_mesh_t get_ctype(jpc::triangle_mesh& m,
                             c::bvh_tree_t*      tree = nullptr)
{
    return {
        .vertices_count = uint(m.vertices.size()),
        .vertices = (c::float3*)m.vertices.data(),
        .normal_count = uint(m.normals.size()),
        .normals = (c::float3*)m.normals.data(),
        .uvs_count = uint(m.uvs.size()),
        .uvs = (c::float2*)m.uvs.data(),
        .faces_count = uint(m.vertices_ids.size()),
        .vertices_ids = (c::uint3*)m.vertices_ids.data(),
        .normals_ids = (c::uint3*)m.normals_ids.data(),
        .uvs_ids = (c::uint3*)m.uvs_ids.data(),
        .material_ids = m.material_ids.data(),
        .bvh_tree = tree,

    };
}

image_t get_ctype(jpc::image& i)
{
    return {
        .data = i.begin(),
        .width = i.width,
        .height = i.height,
        .channels = i.channels,
    };
}

meshes::bvh_tree::bvh_tree(void* tree) : c_tree(tree)
{
}

meshes::bvh_tree::bvh_tree(meshes::bvh_tree&& t) noexcept : c_tree(t.c_tree)
{
    t.c_tree = nullptr;
}

meshes::bvh_tree& meshes::bvh_tree::operator=(meshes::bvh_tree&& t) noexcept
{
    c_tree = t.c_tree;
    t.c_tree = nullptr;
    return *this;
}

meshes::bvh_tree::~bvh_tree()
{
    if (c_tree != nullptr)
    {
        bvhtree_free((bvh_tree_t*)c_tree);
    }
}

mesh meshes::push_back(sphere_mesh&& m)
{
    spheres.push_back(std::forward<sphere_mesh>(m));
    uint          id = uint(spheres.size() - 1);
    sphere_mesh_t c_mesh = get_ctype(spheres[id]);
    sphere_trees.push_back(bvhtree_spheres_build(&c_mesh));

    for (int i = 0; i < spheres.size(); i++)
    {
    }

    return {mesh::JPC_SPHERE, id};
}

mesh meshes::push_back(triangle_mesh&& m)
{
    triangles.push_back(std::forward<triangle_mesh>(m));
    uint            id = uint(triangles.size() - 1);
    triangle_mesh_t c_mesh = get_ctype(triangles[id]);
    triangles_trees.push_back(bvhtree_triangles_build(&c_mesh));
    return {mesh::JPC_TRIANGLE, id};
}

uint meshes::triangle_meshes_size() const
{
    return uint(triangles.size());
}

uint meshes::sphere_meshes_size() const
{
    return uint(spheres.size());
}

void meshes::triangles_ctpyes(void* _dst)
{
    triangle_mesh_t* dst = (triangle_mesh_t*)_dst;
    for (uint i = 0; i < triangle_meshes_size(); i++)
    {
        dst[i]
            = get_ctype(triangles[i], (bvh_tree_t*)triangles_trees[i].c_tree);
    }
}

void meshes::sphere_ctypes(void* _dst)
{
    sphere_mesh_t* dst = (sphere_mesh_t*)_dst;
    for (uint i = 0; i < sphere_meshes_size(); i++)
    {
        dst[i] = get_ctype(spheres[i], (bvh_tree_t*)sphere_trees[i].c_tree);
    }
}

scene::scene()
{
}

material scene::create_material(std::string shader)
{
}

} // namespace jpc