#include "allocators.h"
#include <assert.h>
#include <cglm/cglm.h>
#include <jpc_prefabs.h>
#include <string.h>

#define MAX_INSTANCES 100
#define MAX_SPHERE_MESHES 50
#define MAX_TRIANGLE_MESHES 50

#define MAX_SUN_LIGHTS 5
#define MAX_POINT_LIGHTS 5
#define MESH_BUFFER_SIZE 4096

typedef struct scene_manager_s
{
    scene_t         scene;
    instance_t      instances[MAX_INSTANCES];
    sphere_mesh_t   sphere_meshes[MAX_SPHERE_MESHES];
    triangle_mesh_t triangle_meshes[MAX_TRIANGLE_MESHES];
    point_light_t   point_lights[MAX_POINT_LIGHTS];
    sun_light_t     sun_lights[MAX_SUN_LIGHTS];

    scratch_allocator_t mesh_buffer_allocator;
    uint8_t             mesh_buffer_data[MESH_BUFFER_SIZE];
    arena_t* arena;

} scene_manager_t;

const char* shader_names[] = {
    "Diffuse",
    "Diffuse",
    "Diffuse",
    "Diffuse",
    "Mirror",
    "Mirror",
    "Mirror",
    "Mirror",

};

shader_t* shader_find(shaders_t shaders, const char* name)
{
    for (uint i = 0; i < shaders.count; i++)
    {
        if (strcmp(shaders.shaders[i].name, name) == 0)
        {
            return shaders.shaders + i;
        }
    }
    assert(1);
}

void shader_bfr_fill(shaders_t    shaders,
                     const char** names,
                     shader_t*    dest,
                     uint         n)
{
    for (uint i = 0; i < n; i++)
    {
        dest[i] = *shader_find(shaders, names[i]);
    }
}

static shader_t shader_buffer[MAT_COUNT];

scene_manager_t* scene_manager_init()
{
    scene_manager_t* manager = malloc(sizeof(scene_manager_t));

    manager->arena = arena_make();

    scene_t scene = {
        .camera.clip_end = 400,
        .camera.near_plane = 1,
        .camera.transformation = GLM_MAT4_IDENTITY_INIT,
        .geometries.bvhtree_instances = NULL,
        .geometries.instances = manager->instances,
        .geometries.instances_count = 0,
        .geometries.spheres = manager->sphere_meshes,
        .geometries.sphere_mesh_count = 0,
        .geometries.triangles = manager->triangle_meshes,
        .geometries.triangle_mesh_count = 0,
        .lights.point_lights = manager->point_lights,
        .lights.sun_lights = manager->sun_lights,
        .materiallib.materials_n = MAT_COUNT,
        .materiallib.textures = NULL,
        .materiallib.textures_count = NULL,
        .materiallib.materials = NULL,
    };

    shaders_t shader_system = shaders_init();

    shaders_load_defaults(shader_system);

    shader_bfr_fill(shader_system, shader_names, shader_buffer, MAT_COUNT);

    scene.materiallib.materials
        = materials_init(manager->arena, shader_buffer, MAT_COUNT);

    manager->scene = scene;
    manager->mesh_buffer_allocator.memory = manager->mesh_buffer_data;
    manager->mesh_buffer_allocator.size = MESH_BUFFER_SIZE;
    manager->mesh_buffer_allocator.used = 0;
    return manager;
}

// when instances are changed the scene is invalid
scene_t* scene_manager_get_scene(scene_manager_t* manager)
{
    geometries_t* geoms = &manager->scene.geometries;
    arena_t* arena = manager->arena;

    for (uint i = 0; i < geoms->sphere_mesh_count; i++)
    {
        geoms->spheres[i].bvh_tree = bvhtree_spheres_build(arena,geoms->spheres + i);
    }
    for (uint i = 0; i < geoms->triangle_mesh_count; i++)
    {
        geoms->triangles[i].bvh_tree
            = bvhtree_triangles_build(arena,geoms->triangles + i);
    }
    geoms->bvhtree_instances = bvhtree_instances_build(arena,geoms);

    return &manager->scene;
}


void scene_manager_free(scene_manager_t* manager)
{
    arena_release(manager->arena);
    free(manager);
}

typedef struct
{
    uint   material_ids[6];
    float3 normals[2];
    uint   normals_ids[6];
    float2 uvs[4];
    uint   uvs_ids[6];
    float3 verticies[4];
    uint   verticies_ids[6];

} quad_buffer_t;

void clac_normal(vec3 a, vec3 b, vec3 c, vec3 dest)
{
    vec3 b_m_a, c_m_a;
    glm_vec3_sub(b, a, b_m_a);
    glm_vec3_sub(c, a, c_m_a);
    glm_vec3_cross(b_m_a, c_m_a, dest);
    glm_vec3_normalize(dest);
}

instance_handle_t scene_manager_create_quad(scene_manager_t* manager,
                                            float            points[4][3],
                                            material_id_e    material)
{
    assert(manager->scene.geometries.instances_count + 1 < MAX_INSTANCES);
    assert(manager->scene.geometries.triangle_mesh_count + 1
           < MAX_TRIANGLE_MESHES);

    quad_buffer_t* bfr = scratch_alloc(
        &manager->mesh_buffer_allocator, sizeof(quad_buffer_t), _Alignof(float4));
    triangle_mesh_t mesh = {

        .faces_count = 2,
        .material_ids = bfr->material_ids,
        .normals = &bfr->normals,
        .normals_ids = bfr->normals_ids,
        .normal_count = 2,
        .uvs = bfr->uvs,
        .uvs_count = 4,
        .uvs_ids = bfr->uvs_ids,
        .vertices = bfr->verticies,
        .vertices_count = 4,
        .vertices_ids = bfr->verticies_ids,
        .bvh_tree = NULL,
    };
    for (uint i = 0; i < 6; i++)
        mesh.material_ids[i] = material;

    clac_normal(points[0], points[1], points[2], mesh.normals[0]);
    clac_normal(points[0], points[3], points[2], mesh.normals[1]);
    for (uint i = 0; i < 3; i++)
        mesh.normals_ids[0][i] = 0;

    for (uint i = 0; i < 3; i++)
        mesh.normals_ids[1][i] = 1;

    float uvs[] = {0., 0., 0., 1., 1., 0., 1., 1.};

    memcpy(mesh.uvs, uvs, sizeof(uvs));

    uint3 uvs_ids[] = {
        {0, 1, 2},
        {0, 3, 2},
    };
    memcpy(mesh.uvs_ids, uvs_ids, sizeof(uvs_ids));

    memcpy(mesh.vertices, points, sizeof(float)*4*3);

    uint3 vertices_ids[] = {
        {0, 1, 2},
        {0, 3, 2},
    };
    memcpy(mesh.vertices_ids, vertices_ids, sizeof(vertices_ids));

    geometries_t* geoms = &manager->scene.geometries;

    geoms->triangles[geoms->triangle_mesh_count] = mesh;

    geoms->instances[geoms->instances_count] = (instance_t){
        .mesh_id = geoms->triangle_mesh_count,
        .transformations
        = GLM_MAT4_IDENTITY_INIT,
        .type = JPC_TRIANGLE};

    geoms->triangle_mesh_count++;

    instance_handle_t handle = {.id = geoms->instances_count};

    geoms->instances_count++;
    return handle;
}


instance_handle_t scene_manager_create_sphere(scene_manager_t* manager,
                                              sphere_geometry_t sphere,
                                                  material_id_e material)
{

    geometries_t* geoms = &manager->scene.geometries;
    assert(geoms->instances_count+1<MAX_INSTANCES);
    assert(geoms->sphere_mesh_count+1 < MAX_SPHERE_MESHES);

    sphere_geometry_t* sphere_geom = scratch_alloc( &manager->mesh_buffer_allocator,sizeof(sphere_geometry_t),_Alignof(sphere_geometry_t));

    *sphere_geom = sphere;

    uint* material_ids = scratch_alloc(&manager->mesh_buffer_allocator,sizeof(uint),_Alignof(uint));
    material_ids[0] = material;


    geoms->spheres[geoms->sphere_mesh_count] = (sphere_mesh_t){
        .bvh_tree = NULL,
        .count = 1,
        .geometries = sphere_geom,
        .material_ids = material_ids,
    };

    geoms->instances[geoms->instances_count] = (instance_t){
        .mesh_id = geoms->sphere_mesh_count,
        .transformations = GLM_MAT4_IDENTITY_INIT,
        .type = JPC_SPHERE,
    };
    instance_handle_t handle = {.id = geoms->instances_count};
    geoms->instances_count++;
    geoms->sphere_mesh_count++;    

    return handle;
}

void scene_manager_material_set_uniform(scene_manager_t* manager,
                                        material_id_e    material_id,
                                        const char*      shader_prop,
                                        const float*     value)
{

    if ((int)material_id < 4)
    {
        if (strcmp(shader_prop, "color") == 0)
        {
            material_set_uniform(manager->scene.materiallib.materials
                                     + material_id,
                                 shader_buffer + material_id,
                                 1,
                                 value);
            return;
        }
    }
    assert(1);
}

void scene_manager_create_point_light(scene_manager_t* manager,
                                      point_light_t    light)
{
    lights_t* lights = &manager->scene.lights;
    assert(lights->point_lights_count + 1 < MAX_POINT_LIGHTS);
    lights->point_lights[lights->point_lights_count]
        = light;
    lights->point_lights_count++;
    
}