#pragma once

#include "handle_vector.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <exception>
#include <limits>
#include <memory>
#include <optional>
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

enum geometry_type_t
{
    JPC_SPHERE = 0,
    JPC_TRIANGLE = 1,
};

class image_t
{
  private:
    std::vector<float> data;

    uint m_width = 0;
    uint m_height = 0;
    uint m_channels = 0;

  public:
    uint width() const
    {
        return m_width;
    }
    uint height() const
    {
        return m_height;
    }
    uint channels() const
    {
        return m_channels;
    }

    image_t() = default;

    image_t(uint _width, uint _height, uint _channels)
        : data(_width * _height * _channels, 0.f), m_width(_width),
          m_height(_height), m_channels(_channels){};
    void set_dimensions(uint _width, uint _height, uint _channels)
    {
        m_width = _width;
        m_height = _height;
        m_channels = _channels;
        data.resize(m_width * m_height * m_channels, 0);
    }
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
        assert(pos[0] < m_width);
        assert(pos[1] < m_height);
        return &data[pos[1] * m_width * m_channels + pos[0] * m_channels];
    }
    const float* operator[](std::array<uint, 2> pos) const
    {
        assert(pos[0] < m_width);
        assert(pos[1] < m_height);
        return &data[pos[1] * m_width * m_channels + pos[0] * m_channels];
    }
};

struct image_info
{
    uint channels = 0;
    image_info() = default;
    image_info(const image_t& img) : channels(img.channels())
    {
    }
};

using handle_image = handle<image_t, image_info>;

inline c::image_t ctype(image_t& img)
{
    return c::image_t{
        .data = img.begin(),
        .width = img.width(),
        .height = img.height(),
        .channels = img.channels(),
    };
}

enum value_type
{
    UNSET = 0,
    FLOAT1 = 1,
    FLOAT3 = 3,
    FLOAT4 = 4,
};

template <std::size_t size> float* address(std::array<float, size>& v)
{
    return v.data();
}

inline float* address(float& v)
{
    return &v;
}

template <value_type T> struct value_type_traits
{
    using type = std::array<float, T>;
};
template <> struct value_type_traits<FLOAT1>
{
    using type = float;
};

inline constexpr value_type get_value_type(float)
{
    return FLOAT1;
}

inline constexpr value_type get_value_type(float3)
{
    return FLOAT3;
}

inline constexpr value_type get_value_type(float4)
{
    return FLOAT1;
}

inline value_type get_value_type(handle_image data)
{
    return value_type(data.info.channels);
}

class material_value
{
  public:
  private:
    std::variant<float, float3, float4, handle_image> data;

    value_type type = UNSET;

  public:
    inline material_value() : data(1.f), type(UNSET)
    {
    }
    template <class T>
    explicit material_value(const T& value) : data(value), type(get_value_type(value))
    {
    }

    inline value_type get_type() const
    {
        return type;
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

    template <typename T> bool compatible(const T& v) const
    {
        return get_value_type(v) == type;
    }

    inline uint index() const
    {
        return data.index();
    }

    template <typename Vistor> auto visit(Vistor&& vis)
    {
        return std::visit(std::forward<Vistor>(vis), data);
    }
};

template <value_type type>
typename value_type_traits<type>::type get_shader_uniform(c::shader_t& shader,
                                                          uint uniform_i)
{
    typename value_type_traits<type>::type v;
    c::shader_default_uniform(&shader, uniform_i, address(v));
    return v;
}

inline material_value get_shader_uniform(c::shader_t& shader, uint uniform_i)
{
    switch (shader.uniforms_layout[uniform_i].type)
    {
    case c::JPC_float:
        return material_value{get_shader_uniform<FLOAT1>(shader, uniform_i)};
    case c::JPC_float3:
        return material_value{get_shader_uniform<FLOAT3>(shader, uniform_i)};
    case c::JPC_float4:
        return material_value{get_shader_uniform<FLOAT4>(shader, uniform_i)};
    }
    return material_value();
}

template <typename T>
inline void material_set_property(c::material_t&                         mat,
                           const c::shader_t&                     shader,
                           uint                                   uniform_i,
                           T value)
{
    c::material_set_uniform(&mat, &shader, uniform_i, address(value));
}

inline void material_set_property(c::material_t&     mat,
                           const c::shader_t& shader,
                           uint               uniform_i,
                           handle_image       texture)
{
    c::material_set_texture(&mat, &shader, uniform_i, texture.id);
}

inline void material_set_property(c::material_t&     mat,
                           const c::shader_t& shader,
                           uint               uniform_i,
                           material_value     value)
{
    value.visit(
        [&](auto v) { material_set_property(mat, shader, uniform_i, v); });
}

using shader_t = c::shader_t;
class material
{
  private:
    std::vector<material_value>           values;
    std::unordered_map<std::string, uint> map;
    shader_t                              m_shader;

  public:
    material() = default;
    // I is input iterator of value type pair<std::string,material_value>
    inline material(shader_t shader) : m_shader(shader), values(shader.uniforms_count)
    {
        for (uint i = 0; i < shader.uniforms_count; i++)
        {
            c::uniform_desc_t uniform = shader.uniforms_layout[i];
            std::string       name(uniform.name);
            map[name] = i;
            values[i] = get_shader_uniform(shader, i);
        }
    }
    inline material_value& operator[](const std::string& property)
    {
        if (map.contains(property))
            return values[map[property]];
        else
            throw std::runtime_error("wrong material property");
    }
    inline const material_value& operator[](const std::string& property) const
    {

        if (map.contains(property))
            return values[map.at(property)];
        else
            throw std::runtime_error("wrong material property");
    }
    inline std::size_t size() const
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

    auto begin() const
    {
        return values.begin();
    }
    auto end() const
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

    inline c::shader_t shader() const
    {
        return m_shader;
    }
};
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

    static const geometry_type_t geometry_type = geometry_type_t::JPC_SPHERE;

    using ctype_t = c::triangle_mesh_t;

    std::vector<sphere_geometry> geometries;
    std::vector<uint>            material_ids;
};

inline c::sphere_mesh_t ctype(sphere_mesh& m, c::bvh_tree_t* tree = nullptr)
{
    return c::sphere_mesh_t{
        .count = safe_uint(m.geometries.size()),
        .geometries = (c::sphere_geometry_t*)m.geometries.data(),
        .material_ids = (uint*)m.material_ids.data(),
        .bvh_tree = tree,
    };
}

struct triangle_mesh
{

    static const geometry_type_t  geometry_type = geometry_type_t::JPC_TRIANGLE;
    std::vector<float3>           vertices;
    std::vector<float3>           normals;
    std::vector<float2>           uvs;
    std::vector<uint3>            vertices_ids;
    std::vector<uint3>            normals_ids;
    std::vector<uint3>            uvs_ids;
    std::vector<handle<material>> material_ids;
};

inline c::triangle_mesh_t ctype(triangle_mesh& m, c::bvh_tree_t* tree = nullptr)
{
    return {
        .vertices_count = safe_uint(m.vertices.size()),
        .vertices = (c::float3*)m.vertices.data(),
        .normal_count = safe_uint(m.normals.size()),
        .normals = (c::float3*)m.normals.data(),
        .uvs_count = safe_uint(m.uvs.size()),
        .uvs = (c::float2*)m.uvs.data(),
        .faces_count = safe_uint(m.vertices_ids.size()),
        .vertices_ids = (c::uint3*)m.vertices_ids.data(),
        .normals_ids = (c::uint3*)m.normals_ids.data(),
        .uvs_ids = (c::uint3*)m.uvs_ids.data(),
        .material_ids = (uint*)m.material_ids.data(),
        .bvh_tree = tree,

    };
}

inline c::bvh_tree_t* bvhtree_build(const c::triangle_mesh_t& tris)
{
    return bvhtree_triangles_build(&tris);
}

inline c::bvh_tree_t* bvhtree_build(const c::sphere_mesh_t& sphs)
{
    return c::bvhtree_spheres_build(&sphs);
}

inline c::bvh_tree_t* bvhtree_build(const c::geometries_t& geoms)
{
    return c::bvhtree_instances_build(&geoms);
}

// requries bvhtree_build(ctype(t))-> c::bvh_tree_t*
template <class T> class bvh_mesh
{
  private:
    struct bvh_tree

    {
      private:
        c::bvh_tree_t* c_tree = nullptr;

      public:
        bvh_tree() = default;
        bvh_tree(c::bvh_tree_t* tree) : c_tree(tree)
        {
        }

        bvh_tree& operator=(const bvh_tree& t)
        {

            if (c_tree)
                c::bvhtree_free(c_tree);
            if(t.data())
            {
                c_tree = c::bvhtree_copy(t.data());
            }
            else{
                c_tree = nullptr;    
            }
            return *this;
        }
        bvh_tree& operator=(bvh_tree&& t) noexcept
        {

            if (c_tree)
                c::bvhtree_free(c_tree);
            c_tree = t.c_tree;
            t.c_tree = nullptr;
            return *this;
        }
        bvh_tree(const bvh_tree& t)
        {
            operator=(t);
        };
        bvh_tree(bvh_tree&& t) noexcept
        {
            operator=(t);
        }
        c::bvh_tree_t* data() const
        {
            return c_tree;
        }
        ~bvh_tree()
        {
            if (c_tree)
                c::bvhtree_free(c_tree);
            c_tree=nullptr;
        }
    };
    bvh_tree tree;
    T        mesh;

  public:
    static const geometry_type_t geometry_type = T::geometry_type;

    bvh_mesh() = default;
    bvh_mesh(const bvh_mesh& m) = default;
    bvh_mesh(bvh_mesh&& m) = default;
    bvh_mesh& operator=(const bvh_mesh& m) = default;
    bvh_mesh& operator=(bvh_mesh&& m) = default;
    bvh_mesh(T&& mesh) : mesh(mesh)
    {
        update_bvh();
    }
    const T& get()
    {
        return mesh;
    }
    void set(const T& value)
    {
        mesh = value;
        update_bvh();
    }

    bvh_mesh& operator=(const T& value)
    {
        set(value);
        return *this;
    };

    bvh_mesh& operator=(T&& value)
    {
        set(value);
        return *this;
    };

    void update_bvh()
    {
        auto c_mesh = ctype(mesh);
        tree = bvh_tree(bvhtree_build(c_mesh));
    }

    friend auto ctype(bvh_mesh& m)
    {
        return ctype(m.mesh, m.tree.data());
    }
};

struct mesh_t
{
    geometry_type_t type=geometry_type_t::JPC_SPHERE;
    uint            mesh_id=0;

    mesh_t()=default;
    template <class T>
    inline mesh_t(const handle<T>& m) : type(T::geometry_type), mesh_id(m.id)
    {
    }


    template <class T>
    mesh_t& operator=(const handle<T>& m)
    {
        type=T::geometry_type;
        mesh_id = m.id;
        return *this;
    }
};

struct instance_t
{

    mesh_t                mesh;
    std::array<float, 16> transformation;
};

inline std::unordered_map<std::string, shader_t> shaders_init()
{
    c::shaders_t shaders = c::shaders_init();
    c::shaders_load_defaults(shaders);
    std::unordered_map<std::string, shader_t> dst;
    std::for_each_n(shaders.shaders, shaders.count, [&](const c::shader_t& sh) {
        dst[sh.name] = sh;
    });
    return dst;
}

struct camera_t
{
    float  near_plane = 1;
    float3 position = {0, 0, 0};
    float3 direction = {0, 0, -1};
    float  clip_end = 400;
};

inline c::camera_t ctype(camera_t& cam)
{
    c::camera_t c_cam = *((c::camera_t*)&cam);
    return c_cam;
}

struct point_light_t
{
    float4 color;
    float  strength;
    float  falloff;
    float3 position;
};

struct sun_light_t
{
    float4 color;
    float  strength;
    float3 direction;
};

using mat_bfr_ptr = std::unique_ptr<c::mat_bfr_t, decltype(&c::mat_bfr_t_free)>;

inline mat_bfr_ptr mat_bfr_ctype(const material* materials,
                          c::material_t*  c_materials,
                          uint            n)
{
    std::vector<shader_t> tmp_shaders(n);
    std::transform(materials,
                   materials + n,
                   tmp_shaders.begin(),
                   [](const material& mat) { return mat.shader(); });

    mat_bfr_ptr bfr(c::materials_init(c_materials, tmp_shaders.data(), n),
                    &c::mat_bfr_t_free);

    for (uint i = 0; i < n; i++)
    {
        c::material_t&  c_mat = c_materials[i];
        const material& mat = materials[i];
        c::shader_t&    shader = tmp_shaders[i];
        for (uint uniform_i = 0; i < mat.size(); i++)
        {
            material_value value = mat.begin()[uniform_i];
            material_set_property(c_mat, shader, uniform_i, value);
        }
    }
    return bfr;
}

template <typename T> struct ctype_traits
{
    using type = decltype(ctype(*((T*)nullptr)));
};

template <typename T, typename Info>
std::vector<typename ctype_traits<T>::type> ctype(handle_vector<T, Info>& vec)
{
    using ctype_t = typename ctype_traits<T>::type;
    std::vector<ctype_t> dst(vec.size());
    std::transform(vec.data(),
                   vec.data() + vec.size(),
                   dst.begin(),
                   [&](T& val) { return ctype(val); });
    return dst;
}

class materiallib_cdata
{

    std::vector<c::material_t> c_materials;
    mat_bfr_ptr                mat_bfr = {nullptr, &c::mat_bfr_t_free};
    std::vector<c::image_t>    c_images;

  public:
    materiallib_cdata() = default;
    inline materiallib_cdata(handle_vector<material>&            materials,
                      handle_vector<image_t, image_info>& images)
        :

          c_materials(materials.size()), mat_bfr{mat_bfr_ctype(
                                             materials.data(),
                                             c_materials.data(),
                                             materials.size())},
          c_images{ctype(images)}
    {
    }

    inline friend c::materiallib_t ctype(materiallib_cdata& m)
    {

        return c::materiallib_t{
            .materials_n = safe_uint(m.c_materials.size()),
            .materials = m.c_materials.data(),
            .buffer = &*m.mat_bfr,
            .textures = m.c_images.data(),
            .textures_count = safe_uint(m.c_images.size()),
        };
    }
};

class geometries_cdata
{
    std::vector<c::sphere_mesh_t>   c_sphs;
    std::vector<c::triangle_mesh_t> c_tris;
    c::instance_t*                  c_instances;
    uint                            instances_count = 0;

  public:
    geometries_cdata() = default;
    inline geometries_cdata(handle_vector<bvh_mesh<sphere_mesh>>*   sphs,
                     handle_vector<bvh_mesh<triangle_mesh>>* tris,
                     std::vector<instance_t>&                instances)
        : c_sphs(ctype(*sphs)), c_tris(ctype(*tris)),
          c_instances((c::instance_t*)instances.data()),
          instances_count(safe_uint(instances.size()))
    {
    }

    inline friend c::geometries_t ctype(geometries_cdata& geom,
                                 c::bvh_tree_t*    tree = nullptr)
    {
        return c::geometries_t{
            .instances_count = geom.instances_count,
            .instances = geom.c_instances,
            .triangle_mesh_count = safe_uint(geom.c_tris.size()),
            .triangles = geom.c_tris.data(),
            .sphere_mesh_count = safe_uint(geom.c_sphs.size()),
            .spheres = geom.c_sphs.data(),
            .bvhtree_instances = tree,
        };
    }
};

using geomlib = bvh_mesh<geometries_cdata>;

inline c::lights_t lights_ctype(handle_vector<point_light_t>& point_lights,
                         handle_vector<sun_light_t>&   sun_lights)
{
    return c::lights_t{
        .point_lights_count = point_lights.size(),
        .point_lights = (c::point_light_t*)point_lights.data(),
        .sun_lights_count = sun_lights.size(),
        .sun_lights = (c::sun_light_t*)sun_lights.data(),
    };
}

struct scene_t
{
    handle_vector<bvh_mesh<sphere_mesh>>      spheres;
    handle_vector<bvh_mesh<triangle_mesh>>    triangles;
    std::vector<instance_t>                   instances;
    handle_vector<material>                   materials;
    handle_vector<image_t, image_info>        textures;
    std::unordered_map<std::string, shader_t> shaders;
    camera_t                                  camera;
    handle_vector<point_light_t>              point_lights;
    handle_vector<sun_light_t>                sun_lights;

    inline scene_t()
    {
        shaders = shaders_init();
        shader_t default_shader = shaders["diffuse"];
        materials.default_element() = material(default_shader);
    }

  private:
    bvh_mesh<geometries_cdata> c_geoms;
    materiallib_cdata          c_mats;

  public:
    inline friend c::scene_t ctype(scene_t& scn)
    {
        scn.c_geoms
            = geometries_cdata(&scn.spheres, &scn.triangles, scn.instances);
        scn.c_mats = materiallib_cdata(scn.materials, scn.textures);
        c::scene_t c_scn = {
            .geometries = ctype(scn.c_geoms),
            .materiallib = ctype(scn.c_mats),
            .camera = ctype(scn.camera),
            .lights = lights_ctype(scn.point_lights, scn.sun_lights),
        };
        return c_scn;
    }
};

struct render_settings_t
{

    uint tile_size = 32;
    uint subpixels = 4;
    uint light_samples = 100;
    uint max_depth = 5;
};

inline c::render_settings_t ctype(const render_settings_t& settings)
{
    c::render_settings_t c_settings = *((c::render_settings_t*)&settings);
    return c_settings;
}

inline image_t render(scene_t&                 scn,
               const render_settings_t& settings,
               uint                     width = 200,
               uint                     height = 200)
{
    image_t              dst(width, height, 4);
    c::image_t           c_dst = ctype(dst);
    c::render_settings_t c_settings = ctype(settings);
    c::scene_t           c_scn = ctype(scn);
    c::render(&c_scn, c_settings, &c_dst);
    return dst;
}

} // namespace jpc
