#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace jpc
{

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

struct sphere_mesh
{
    std::vector<sphere_geometry> geometries;
    std::vector<uint>            material_ids;
};

struct triangle_mesh
{
    std::vector<float3> vertices;
    std::vector<float3> normals;
    std::vector<float2> uvs;
    std::vector<uint3>  vertices_ids;
    std::vector<uint3>  uvs_ids;
    std::vector<uint>   material_ids;
};

struct mesh_t
{

    typedef enum
    {
        JPC_SPHERE = 0,
        JPC_TRIANGLE = 1,
    } geometry_type_t;
    geometry_type_t type;
    uint            mesh_id;
};

struct bvh_tree
{
    void* c_tree;
    bvh_tree(void* tree);
    ~bvh_tree();
};

struct instance
{
    mesh_t                mesh;
    std::array<float, 16> transformations;
};

class meshes
{

  private:
    std::vector<const sphere_mesh>   spheres;
    std::vector<const triangle_mesh> triangles;
    std::vector<bvh_tree>            sphere_trees;
    std::vector<bvh_tree>            triangles_trees;

  public:
    mesh_t push_back(sphere_mesh&& mesh);
    mesh_t push_back(triangle_mesh&& mesh);
};

class image
{
  private:
    std::vector<float> data;

  public:
    const uint width;
    const uint height;
    const uint channels;
    image(uint width, uint height, uint channels)
        : width(width), height(height), channels(channels),
          data(width * height * channels, 0.f){};
    float* begin();
    float* end();

    const float* cbegin() const;
    const float* cend() const;

    float*       operator[](std::array<float, 2> pos);
    const float* operator[](std::array<float, 2> pos) const;
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
    material_value(float a);
    material_value(float3 a);
    material_value(float4 a);

    const value_type type;

    uint channels() const
    {
        uint res[] = {1, 3, 4};
        return res[type];
    }
    template <typename T> void operator=(const T& a)
    {
        if (compatible(a))
        {
            data = a;
        }
        else
        {
            throw std::bad_variant_access();
        }
    }

    template <typename T> T& get()
    {
        return data.get<T>();
    }

    template <typename T> const T& get() const
    {
        return data.get<T>();
    };

    template <typename T> bool compatible(T val) const
    {
        return false;
    }

    bool compatible(float a) const
    {
        return type == FLOAT1;
    }

    bool compatible(float3 a) const
    {
        return type == FLOAT3;
    }
    bool compatible(float4 a) const
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
    material_value&       operator[](std::string property);
    const material_value& operator[](std::string property) const;
    std::size_t           size() const;

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
        std::vector<image> textures;
        std::vector<material> materials;
    public:
    std::vector<instance> instances;
    meshes                meshes;
    
    std::vector<std::string> shader_names;

    material  create_material(std::string shader);

    image_ref push_back(image&& texture);
    uint      push_back(material&& material);

    auto textures_begin(){return textures.begin();}
    auto textures_end(){return textures.end();}
    auto materials_being(){return materials.begin();}
    auto materials_end(){return materials.end();}

};

} // namespace jpc
