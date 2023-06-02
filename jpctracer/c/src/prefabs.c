#include<jpc_prefabs.h>
#include<string.h>
#include<assert.h>
#include<cglm/cglm.h>
#include"allocators.h"

#define MAX_INSTANCES 100
#define MAX_SPHERE_MESHES 50
#define MAX_TRIANGLE_MESHES 50
#define MAX_SPHERES MAX_SPHERE_MESHES*10
#define MAX_TRIANGELS MAX_TRIANGLE_MESHES*100

#define MAX_SUN_LIGHTS 5
#define MAX_POINT_LIGHTS 5
#define MESH_BUFFER_SIZE 4096


typedef struct
{
    scene_t scene;
    instance_t instances[MAX_INSTANCES];
    sphere_mesh_t sphere_meshes[MAX_SPHERE_MESHES];
    triangle_mesh_t triangle_meshes[MAX_TRIANGLE_MESHES];
    point_light_t   point_lights[MAX_POINT_LIGHTS];
    sun_light_t     sun_lights[MAX_SUN_LIGHTS];
    material_t      materials[MAX_INSTANCES];
    
    
    scratch_allocator_t mesh_buffer_allocator;
    uint8_t mesh_buffer_data[MESH_BUFFER_SIZE];



}scene_manager_t;

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

void shader_bfr_fill(shaders_t shaders, const char** names, shader_t* dest, uint n)
{
    for (uint i = 0; i < n; i++)
    {
        dest[i] = *shader_find(shaders, names[i]);
    }
}

scene_manager_t* scene_manager_init()
{
    scene_manager_t* manager = malloc(sizeof(scene_manager_t));
    
    scene_t scene = {
        .camera.clip_end = 400,
        .camera.near_plane = 1,
        .camera.position = {0, 0, 0},
        .geometries.bvhtree_instances = NULL,
        .geometries.instances = manager->instances,
        .geometries.instances_count = 0,
        .geometries.spheres = manager->sphere_meshes,
        .geometries.sphere_mesh_count = 0,
        .geometries.triangles = manager->triangle_meshes,
        .geometries.triangle_mesh_count = 0,
        .lights.point_lights = manager->point_lights,
        .lights.sun_lights = manager->sun_lights,
        .materiallib.buffer = NULL,
        .materiallib.materials = manager->materials,
        .materiallib.materials_n = MAT_COUNT,
        .materiallib.textures = NULL,
        .materiallib.textures_count = NULL,

    };

    const shader_t shader_buffer[MAT_COUNT];

    shaders_t shader_system = shaders_init();

    shaders_load_defaults(shader_system);

    shader_bfr_fill(shader_system, shader_names, shader_buffer, MAT_COUNT);
    


    scene.materiallib.buffer = materials_init(scene.materiallib.materials, shader_buffer, MAT_COUNT);

    manager->scene = scene;
    manager->mesh_buffer_allocator.memory = manager->mesh_buffer_data;
    manager->mesh_buffer_allocator.size = MESH_BUFFER_SIZE;
    manager->mesh_buffer_allocator.used = 0;
    return manager;
}

// when instances are changed the scene is invalid
scene_t* scene_manager_get_scene(scene_manager_t* manager)
{
    geometries_t geoms = manager->scene.geometries;

    for (uint i = 0; i < geoms.sphere_mesh_count; i++)
    {
        geoms.spheres[i].bvh_tree = bvhtree_spheres_build(geoms.spheres+i);
    }
    for (uint i = 0; i < geoms.triangle_mesh_count; i++)
    {
        geoms.triangles[i].bvh_tree
            = bvhtree_triangles_build(geoms.triangles + i);
    }
    geoms.bvhtree_instances = bvhtree_instances_build(&geoms);

}

void geoms_free_all_bvhtrees(geometries_t geoms)
{
    if (geoms.bvhtree_instances)
    {
        bvhtree_free(geoms.bvhtree_instances);
    }
    for (uint i = 0; i < geoms.sphere_mesh_count; i++)
    {
        if (geoms.spheres[i].bvh_tree)
        {
            bvhtree_free(geoms.spheres[i].bvh_tree);
        }
    }
    for (uint i = 0; i < geoms.triangle_mesh_count; i++)
    {
        if (geoms.triangles[i].bvh_tree)
        {
        bvhtree_free(geoms.triangles[i].bvh_tree);
        }
    }
}

void scene_manager_free(scene_manager_t* manager)
{
    geometries_t geoms = manager->scene.geometries;

    geoms_free_all_bvhtrees(geoms);
    mat_bfr_t_free(manager->scene.materiallib.buffer);

    free(manager);
}

typedef struct
{
    uint material_ids[6];
    float3 normals[2];
    uint   normals_ids[6];
    float2 uvs[4];
    uint   uvs_ids[6];
    float3 verticies[4];
    uint   verticies_ids[6];


}quad_buffer_t;



instance_handle_t scene_manager_create_quad(scene_manager_t* manager,
    float            points[4][3],
                                            material_id_e    material)
{
    assert(manager->scene.geometries.instances_count<MAX_INSTANCES);
    assert(manager->scene.geometries.triangle_mesh_count< MAX_TRIANGLE_MESHES);

    quad_buffer_t* bfr = scratch_alloc(
        &manager->mesh_buffer_allocator, sizeof(quad_buffer_t), 0);
    triangle_mesh_t mesh = {
        
        .faces_count = 6,
        .material_ids = bfr->material_ids,
        .normals = &bfr->normals,
        .normals_ids = bfr->normals_ids,
        .normal_count = 1,
        .uvs = bfr->uvs,
        .uvs_count = 4,
        .uvs_ids = bfr->uvs_ids,
        .vertices = bfr->verticies,
        .vertices_count = 4,
        .vertices_ids = bfr->verticies_ids,
    };
    for (uint i = 0; i < 6; i++)
        mesh.material_ids[i] = material;

    
}

