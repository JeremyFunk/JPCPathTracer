#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <exception>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace jpc
{

namespace c
{
extern "C"
{
#include "jpc_api.h"
}
} // namespace c

using uint = std::uint32_t;
using uint3 = std::array<uint, 3>;
using float3 = std::array<float, 3>;
using float4 = std::array<float, 4>;
using float2 = std::array<float, 2>;

struct sphere_geometry
{
    float3 position;
    float  radius;
};

template <class T> uint safe_uint(const T& a)
{
    assert(a < std::numeric_limits<uint>::max());
    assert(a >= std::numeric_limits<uint>::min());
    return uint(a);
}

struct sphere_mesh
{

    using ctype_t = c::triangle_mesh_t;

    std::vector<sphere_geometry> geometries;
    std::vector<uint>            material_ids;

    c::sphere_mesh_t ctype(c::bvh_tree_t* tree = nullptr)
    {
        return c::sphere_mesh_t{
            .count = safe_uint(geometries.size()),
            .geometries = (c::sphere_geometry_t*)geometries.data(),
            .material_ids = material_ids.data(),
            .bvh_tree = tree,
        };
    }
};

struct triangle_mesh
{

    using ctype_t = c::triangle_mesh_t;

    std::vector<float3> vertices;
    std::vector<float3> normals;
    std::vector<float2> uvs;
    std::vector<uint3>  vertices_ids;
    std::vector<uint3>  normals_ids;
    std::vector<uint3>  uvs_ids;
    std::vector<uint>   material_ids;

    c::triangle_mesh_t ctype(c::bvh_tree_t* tree = nullptr)
    {
        return {
            .vertices_count = safe_uint(vertices.size()),
            .vertices = (c::float3*)vertices.data(),
            .normal_count = safe_uint(normals.size()),
            .normals = (c::float3*)normals.data(),
            .uvs_count = safe_uint(uvs.size()),
            .uvs = (c::float2*)uvs.data(),
            .faces_count = safe_uint(vertices_ids.size()),
            .vertices_ids = (c::uint3*)vertices_ids.data(),
            .normals_ids = (c::uint3*)normals_ids.data(),
            .uvs_ids = (c::uint3*)uvs_ids.data(),
            .material_ids = material_ids.data(),
            .bvh_tree = tree,

        };
    }
};

c::bvh_tree_t* bvhtree_build(const c::triangle_mesh_t& tris)
{
    return bvhtree_triangles_build(&tris);
}

c::bvh_tree_t* bvhtree_build(const c::sphere_mesh_t& sphs)
{
    return c::bvhtree_spheres_build(&sphs);
}

struct mesh
{

    typedef enum
    {
        JPC_SPHERE = 0,
        JPC_TRIANGLE = 1,
    } geometry_type_t;
    geometry_type_t type;
    uint            mesh_id;
};

template <class T> mesh::geometry_type_t get_geometry_type();

template <> mesh::geometry_type_t get_geometry_type<sphere_mesh>()
{
    return mesh::JPC_SPHERE;
}

template <> mesh::geometry_type_t get_geometry_type<triangle_mesh>()
{
    return mesh::JPC_TRIANGLE;
}

struct instance
{
    mesh                  mesh;
    std::array<float, 16> transformations;
};

// requries
//      t.ctype()
//      typename T::ctpye_t;
//      t.ctpye(c::bvh_tree_t*)
//      bvhtree_build(t.ctype())
//
template <class T> class meshes
{

  public:
    using mesh_ctype_t = typename T::ctype_t;
    using ctype_t = mesh_ctype_t*;

  private:
    struct bvh_tree

    {
        c::bvh_tree_t* c_tree = nullptr;
        bvh_tree(c::bvh_tree_t* tree) : c_tree(tree)
        {
        }
        bvh_tree(const bvh_tree& t) = delete;
        bvh_tree(bvh_tree&& t) noexcept : c_tree(t.c_tree)
        {
            t.c_tree = nullptr;
        }
        bvh_tree& operator=(const bvh_tree& t) = delete;
        bvh_tree& operator=(bvh_tree&& t) noexcept
        {
            if (c_tree)
                c::bvhtree_free(c_tree);
            c_tree = t.c_tree;
            t.c_tree = nullptr;
            return *this;
        }
        ~bvh_tree()
        {
            if (c_tree)
                c::bvhtree_free(c_tree);
        }
    };
    std::vector<T>        mesh_data;
    std::vector<bvh_tree> trees;

    std::vector<mesh_ctype_t> c_mesh_data;

  public:
    meshes() = default;
    meshes(meshes&& m) noexcept = default;
    meshes(const meshes& m) = delete;
    meshes& operator=(const meshes& m) = delete;
    meshes& operator=(meshes&& m) noexcept = default;
    ~meshes() = default;

    mesh push_back(T&& m)
    {
        uint id = safe_uint(mesh_data.size());
        mesh_data.push_back(m);
        mesh_ctype_t   c_mesh = mesh_data[id].ctype();
        c::bvh_tree_t* tree = bvhtree_build(&c_mesh);
        trees.push_back(tree);
        return mesh{.type = get_geometry_type<T>(), .mesh_id = id};
    }

    uint size() const
    {
        return mesh_data.size();
    }

    auto begin() const
    {
        return mesh_data.cbegin();
    }
    auto end() const
    {
        return mesh_data.cend();
    }

    ctype_t ctpye()
    {
        c_mesh_data.resize(mesh_data.size());
        for (std::size_t i = 0; i < mesh_data.size(); i++)
        {
            c_mesh_data[i] = mesh_data[i].ctype(trees[i].c_tree);
        }
        return c_mesh_data.data();
    }
};

class image
{
  private:
    std::vector<float> data;

  public:
    using ctype_t = c::image_t;

    const uint width;
    const uint height;
    const uint channels;

    image(uint _width, uint _height, uint _channels)
        : data(_width * _height * _channels, 0.f), width(_width),
          height(_height), channels(_channels){};
    float* begin()
    {
        return &data[0];
    }
    float* end()
    {
        return begin() + data.size();
    }

    const float* cbegin() const
    {
        return &data[0];
    }
    const float* cend() const
    {
        return cbegin() + data.size();
    }

    // x,y
    float* operator[](std::array<uint, 2> pos)
    {
        assert(pos[0] < width);
        assert(pos[1] < height);
        return &data[pos[1] * width * channels + pos[0] * channels];
    }
    const float* operator[](std::array<uint, 2> pos) const
    {
        assert(pos[0] < width);
        assert(pos[1] < height);
        return &data[pos[1] * width * channels + pos[0] * channels];
    }

    ctype_t ctype()
    {
        return c::image_t{
            .data = begin(),
            .width = width,
            .height = height,
            .channels = channels,
        };
    }
};

struct image_ref
{
    uint id;
    uint channels;
};

class material_value
{
  public:
    enum value_type
    {
        FLOAT1 = 0,
        FLOAT3 = 1,
        FLOAT4 = 2,
    };

  private:
    std::variant<float, float3, float4, image_ref> data;

  public:
    material_value(float a) : data(a), type(FLOAT1)
    {
    }
    material_value(float3 a) : data(a), type(FLOAT3)
    {
    }
    material_value(float4 a) : data(a), type(FLOAT4)
    {
    }

    const value_type type;

    uint channels() const
    {
        uint res[] = {1, 3, 4};
        return res[type];
    }
    template <typename T> material_value& operator=(const T& a)
    {
        if (compatible(a))
        {
            data = a;
        }
        else
        {
            throw std::bad_variant_access();
        }
        return *this;
    }

    template <typename T> T& get()
    {
        return std::get<T>(data);
    }

    template <typename T> const T& get() const
    {
        return std::get<T>(data);
    };

    template <typename T> bool compatible(T) const
    {
        return false;
    }

    bool compatible(float) const
    {
        return type == FLOAT1;
    }

    bool compatible(float3) const
    {
        return type == FLOAT3;
    }
    bool compatible(float4) const
    {
        return type == FLOAT4;
    }
    bool compatible(image_ref a) const
    {
        return a.channels == channels();
    }
};

class material
{
  private:
    std::unordered_map<std::string, material_value> values;

  public:
    template <typename I>
    // I is input iterator of value type pair<std::string,material_value>
    material(I first, I last) : values(first, last)
    {
    }
    material_value& operator[](const std::string& property)
    {
        if (values.contains(property))
            return values[property];
        else
            throw std::runtime_error("wrong material property");
    }
    const material_value& operator[](const std::string& property) const
    {
        if (values.contains(property))
            return values.at(property);
        else
            throw std::runtime_error("wrong material property");
    }
    std::size_t size() const
    {
        return values.size();
    }

    auto begin()
    {
        return values.begin();
    }
    auto end()
    {
        return values.end();
    }

    auto cbegin() const
    {
        return values.cbegin();
    }
    auto cend() const
    {
        return values.cend();
    }
};

class scene
{
  private:
    std::vector<image>    textures;
    std::vector<material> materials;

    c::shaders_t c_shaders;

    std::unordered_map<std::string, c::shader_t> shaders;

  public:
    scene()
    {
        c_shaders = c::shaders_init();
        c::shaders_load_defaults(c_shaders);
        for (uint i = 0; i < c_shaders.count; i++)
        {
            c::shader_t shader = c_shaders.shaders[i];
            shaders[std::string(shader.name)] = shader;
        }
    }
    std::vector<instance> instances;
    meshes<sphere_mesh>   spheres;
    meshes<triangle_mesh> triangles;

    material create_material(std::string shader)
    {
    }

    image_ref push_back(image&& texture);
    uint      push_back(material&& material);

    auto textures_begin()
    {
        return textures.begin();
    }
    auto textures_end()
    {
        return textures.end();
    }
    auto materials_being()
    {
        return materials.begin();
    }
    auto materials_end()
    {
        return materials.end();
    }
    ~scene()
    {
    }
};

} // namespace jpc
