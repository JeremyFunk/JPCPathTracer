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
    JPC_float = 1,
    JPC_float3 = 3,
    JPC_float4 = 4,
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

typedef unsigned int uint;
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

// requirese width*height*channels < UINT32_MAX
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
    uint3* vertices_ids;
    uint3* normals_ids;
    uint3* uvs_ids;
    uint*  material_ids;

    bvh_tree_t* bvh_tree;

} triangle_mesh_t;

typedef struct
{
    float3 position;
    float  radius;
} sphere_geometry_t;

typedef struct
{
    uint               count;
    sphere_geometry_t* geometries;
    uint*              material_ids;
    bvh_tree_t*        bvh_tree;

} sphere_mesh_t;

typedef struct
{
    geometry_type_t type;
    uint            mesh_id;

    float4x4 transformations;
} instance_t;

typedef struct
{
    uint        instances_count;
    instance_t* instances;

    uint             triangle_mesh_count;
    triangle_mesh_t* triangles;

    uint           sphere_mesh_count;
    sphere_mesh_t* spheres;

    bvh_tree_t* bvhtree_instances;
} geometries_t;

typedef struct
{
    float  near_plane;
    float3 position;
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



typedef struct
{
    char* location;
    char* message;
} parsing_error_t;

typedef struct
{
    parsing_error_t* missing;
    uint missing_count;

    parsing_error_t* bad_conversion;
    uint             bad_conversion_count;
    
    parsing_error_t  critical;

} parsing_errors_t;

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

void render(const scene_t*          scene,
            const render_settings_t settings,
            image_t*                outputs);

bvh_tree_t* bvhtree_triangles_build(const triangle_mesh_t* tris);
bvh_tree_t* bvhtree_spheres_build(const sphere_mesh_t* spheres);
bvh_tree_t* bvhtree_instances_build(const geometries_t* geometries);

void bvhtree_free(bvh_tree_t* tree);

parsing_errors_t scene_load_yaml(const char* path, scene_t* dest);

void parsing_errors_free(parsing_errors_t errors);
int             parsing_errors_is_critical(const parsing_errors_t* errors);
void parsing_errors_print(const parsing_errors_t* errors, int bad_conversion, int missing);


bvh_tree_t* bvhtree_copy(const bvh_tree_t* tree);
