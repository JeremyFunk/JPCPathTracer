#pragma once
/*
Uniforms:
ID Type Name Offset
*/

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef enum
{
    JPC_float = 0,
    JPC_float4 = 1,
    JPC_float3 = 2,
} uniform_type_t;

typedef enum
{
    JPC_SPHERE = 0,
    JPC_TRIANGLE = 1,
} geometry_type_t;

// forward declaration
typedef struct bvh_tree_s    bvh_tree_t;
typedef struct bsdfnode_s    bsdfnode_t;
typedef struct bsdfcontext_s bsdfcontext_t;

// Shaders
// ID Name Parameter Size Uniform Range

typedef unsigned int uint2[2];
typedef unsigned int uint3[3];
typedef float        float4[4];
typedef float        float3[3];
typedef float        float2[2];
typedef float        float4x4[4][4];

typedef bsdfnode_t (*create_bsdf_f)(bsdfcontext_t* ctx, void* parms);

typedef struct
{
    uniform_type_t type;
    const char*    name;

} uniform_desc_t;

typedef struct shader_s
{
    const char*           name;
    const uniform_desc_t* uniforms_layout;
    uint                  uniforms_count;
    size_t                uniforms_size;
    const void*           uniforms_default;
    create_bsdf_f         create_bsdf;

} shader_t;

typedef struct shaders_s
{
    shader_t* shaders;
    uint      count;
} shaders_t;

// Uniforms:
// uniform id | value/color/normal/texture_id

typedef struct
{
    float* data;
    uint   width;
    uint   height;
    uint   channels;
} image_t;

typedef struct
{
    int            texture;
    uniform_type_t type;
    uintptr_t      offset;
} texture_uniform_binding_t;

// Material
typedef struct
{
    create_bsdf_f              bsdf_creator;
    uint                       param_size;
    void*                      params;
    texture_uniform_binding_t* bindings;
    uint                       bindings_count;

} material_t;

typedef struct
{
    uint    vertices_count;
    float3* vertices;

    uint    normal_count;
    float3* normals;

    uint    uvs_count;
    float2* uvs;

    uint   faces_count;
    uint3* verticies_ids;
    uint3* normals_ids;
    uint3* uvs_ids;
    uint*  material_slots;

} triangle_mesh_t;

typedef struct
{
    uint    count;
    float3* positions;
    float*  radii;
    uint*   material_slot_id;

    uint  mesh_count;
    uint* mesh_end_idx;
} spheres_t;

typedef struct
{
    geometry_type_t type;
    uint            mesh_id;
    uint            material_slot_start;

    float4x4 transformations;
} instance_t;

typedef struct
{
    uint        instances_count;
    instance_t* instances;
    triangles_t triangles;
    spheres_t   spheres;
    uint*       material_slots;
    uint        material_slots_count;
    bvh_tree_t* bvhtree_instances;
    bvh_tree_t* bvhtree_triangles;
    bvh_tree_t* bvhtree_spheres;
} geometries_t;

typedef struct
{
    float  near_plane;
    float3 position;
    float3 direction;
    float  clip_end;
} camera_t;

typedef struct mat_bfr_s mat_bfr_t;

typedef struct
{
    uint        materials_n;
    material_t* materials;
    mat_bfr_t*  buffer;

    image_t* textures;
    uint     textures_count;
} materiallib_t;

typedef struct
{
    float4 color;
    float  strength;
    float  falloff;
    float3 position;

} point_light_t;

typedef struct
{
    float4 color;
    float  strength;
    float3 direction;
} sun_light_t;

typedef struct
{
    uint           point_lights_count;
    point_light_t* point_lights;
    uint           sun_lights_count;
    sun_light_t*   sun_lights;
} lights_t;

typedef struct scene_s
{
    geometries_t  geometries;
    materiallib_t materiallib;
    camera_t      camera;
    lights_t      lights;

} scene_t;

typedef struct
{
    uint tile_size;
    uint subpixels;
    uint light_samples;
    uint max_depth;
} render_settings_t;

shaders_t shaders_init();
void      shaders_load_defaults(shaders_t shaders);
void      shaders_free(shaders_t shaders);
void      shader_default_uniform(const shader_t* shader, uint id, float* dst);

// returns buffer which contains the params and texturebinding
mat_bfr_t* materials_init(material_t*     materials,
                          const shader_t* shaders,
                          uint            n);

void mat_bfr_t_free(mat_bfr_t* bfr);

// value float, float3 or float4 depended on the type of the uniform
void material_set_uniform(material_t*     mat,
                          const shader_t* shader,
                          uint            uniform_id,
                          float*          value);
void material_set_texture(material_t*     mat,
                          const shader_t* shader,
                          uint            uniform_id,
                          uint            texture);

void geometries_create_bvhtree(geometries_t* geometries);

void render(const scene_t*          scene,
            const render_settings_t settings,
            image_t*                outputs);

void bvhtree_triangles_free(geometries_t* geometries);
void bvhtree_spheres_free(geometries_t* geometries);
void bvhtree_instances_free(geometries_t* geometries);

void bvhtree_triangles_build(geometries_t* geometries);
void bvhtree_spheres_build(geometries_t* geometries);
void bvhtree_instances_build(geometries_t* geometries);
