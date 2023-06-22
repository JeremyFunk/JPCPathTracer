#include "jpc_api.h"
#include "ufbx/ufbx.h"
#include <stdio.h>
#include <string.h>
#include<assert.h>
#include<cglm/cglm.h>

lights_t lights_load_fbx(arena_t* arena, ufbx_light_list fbx_lights)
{
    lights_t lights = {
        .point_lights = NULL,
        .point_lights_count = 0,
        .sun_lights = NULL,
        .sun_lights_count = 0,
    };

    for (int i = 0; i < fbx_lights.count; i++)
    {
        switch (fbx_lights.data[i]->type)
        {
        case UFBX_LIGHT_POINT:
            lights.point_lights_count += fbx_lights.data[i]->instances.count;
            break;
        case UFBX_LIGHT_DIRECTIONAL:
            lights.sun_lights_count += fbx_lights.data[i]->instances.count;
            break;

        default:
            break;
        }
    }

    lights.point_lights
        = arena_alloc(arena, lights.point_lights_count * sizeof(point_light_t));
    lights.sun_lights
        = arena_alloc(arena, lights.sun_lights_count * sizeof(sun_light_t));
    {
        int point_light_i = 0;
        for (int i = 0; i < fbx_lights.count; i++)
        {
            ufbx_light* fbx_light = fbx_lights.data[i];
            if (fbx_lights.data[i]->type == UFBX_LIGHT_POINT)
            {
                point_light_t point_light;

                for (int j = 0; j < 3; j++)
                {
                    point_light.color[j] = fbx_light->color.v[j];
                }
                point_light.color[3] = 1;
                point_light.strength = fbx_light->intensity*200.;
                point_light.falloff = fbx_light->decay - UFBX_LIGHT_DECAY_NONE;

                for (int inst_i = 0; inst_i < fbx_light->instances.count;
                     inst_i++)
                {
                    ufbx_matrix transform_mat
                        = fbx_light->instances.data[inst_i]->node_to_world;
                    ufbx_transform transform
                        = ufbx_matrix_to_transform(&transform_mat);

                    for (int i = 0; i < 3; i++)
                    {
                        point_light.position[i] = transform.translation.v[i];
                    }

                    lights.point_lights[point_light_i] = point_light;

                    point_light_i++;
                }
            }
        }
    }

    {
        int sun_light_i = 0;
        for (int i = 0; i < fbx_lights.count; i++)
        {
            ufbx_light* fbx_light = fbx_lights.data[i];
            if (fbx_lights.data[i]->type == UFBX_LIGHT_DIRECTIONAL)
            {
                sun_light_t sun_light;

                for (int j = 0; i < 3; j++)
                {
                    sun_light.color[j] = fbx_light->color.v[j];
                }
                sun_light.color[3] = 1;
                sun_light.strength = fbx_light->intensity*100.*100.;
                // Direction the light is aimed at in node's local space
                ufbx_vec3 local_direction = fbx_light->local_direction;

                for (int inst_i = 0; inst_i < fbx_light->instances.count;
                     inst_i++)
                {
                    ufbx_matrix transform_mat
                        = fbx_light->instances.data[inst_i]->node_to_world;
                    
                    ufbx_vec3 direction = ufbx_transform_direction(&transform_mat,local_direction);

                    for (int i = 0; i < 3; i++)
                    {
                        sun_light.direction[i] = direction.v[i];
                    }

                    lights.sun_lights[sun_light_i] = sun_light;

                    sun_light_i++;
                }
            }
        }
    }

    return lights;
}

void geometries_fill_index_buffer(ufbx_face_list fbx_faces, ufbx_uint32_list fbx_indices, uint3* dest_face_indices)
{
    for(uint i = 0; i<fbx_faces.count;i++)
    {
        for(uint j = 0; j<3;j++)
        {
            assert(fbx_faces.data[i].num_indices==3 && "Faces must be triangulated");
            uint index = fbx_faces.data[i].index_begin+j;
            dest_face_indices[i][j] = fbx_indices.data[index];
        }
    }
}

ufbx_transform_to_jpc_transform(ufbx_matrix* ufbx_mat, float4x4 jpc_mat)
{

    mat4 temp; 

    for(uint i = 0; i<4;i++)
    {
        for(uint j = 0; j<3;j++)
        {
            temp[i][j] = ufbx_mat->cols[i].v[j];
        }
    }
    temp[0][3] = 0;
    temp[1][3] = 0;
    temp[2][3] = 0;
    temp[3][3] = 1;



    glm_mat4_ucopy(temp,jpc_mat);
}

geometries_t geometries_load_fbx(arena_t* arena, ufbx_mesh_list fbx_meshes)
{
    geometries_t geoms = {
        .bvhtree_instances = NULL,
        .instances = NULL,
        .instances_count = NULL,
        .sphere_mesh_count = 0,
        .spheres = NULL,
        .triangle_mesh_count = 0,
        .triangles = NULL,
    };

    geoms.triangle_mesh_count = fbx_meshes.count;

    for(uint i = 0; i<fbx_meshes.count;i++)
    {
        geoms.instances_count+=fbx_meshes.data[i]->instances.count;
    }

    geoms.instances = arena_alloc(arena,sizeof(instance_t)*geoms.instances_count) ;
    geoms.triangles = arena_alloc(arena,sizeof(triangle_mesh_t)*geoms.triangle_mesh_count);

    uint instance_i = 0;

    for(uint mesh_i = 0; mesh_i<fbx_meshes.count;mesh_i++)
    {
        triangle_mesh_t* mesh = &geoms.triangles[mesh_i];
        ufbx_mesh* fbx_mesh = fbx_meshes.data[mesh_i];

        mesh->faces_count = fbx_mesh->faces.count;
        mesh->normal_count = fbx_mesh->vertex_normal.values.count;
        mesh->uvs_count = fbx_mesh->vertex_uv.values.count;
        mesh->vertices_count = fbx_mesh->vertices.count;

        size_t verticies_size = sizeof(float3)*mesh->vertices_count;
        size_t normals_size = sizeof(float3)*mesh->normal_count;
        size_t uvs_size = sizeof(float2)*mesh->uvs_count;
        size_t verticies_ids_size = sizeof(uint3)*mesh->faces_count;
        size_t normals_ids_size = sizeof(uint3)*mesh->faces_count;
        size_t uvs_ids_size = sizeof(uint3)*mesh->faces_count;
        size_t material_ids_size = sizeof(uint3)*mesh->faces_count;

        arena_reserve(arena,verticies_size+
        normals_size+
        uvs_size+
        verticies_ids_size+
        normals_ids_size+
        uvs_ids_size+
        material_ids_size
        );

        mesh->vertices = arena_alloc(arena,verticies_size);
        mesh->normals = arena_alloc(arena,normals_size);
        mesh->uvs = arena_alloc(arena,uvs_size);
        mesh->vertices_ids = arena_alloc(arena,verticies_ids_size);
        mesh->normals_ids = arena_alloc(arena,normals_ids_size);
        mesh->uvs_ids = arena_alloc(arena,uvs_ids_size);
        mesh->material_ids = arena_alloc(arena,material_ids_size);


        geometries_fill_index_buffer(fbx_mesh->faces,fbx_mesh->vertex_position.indices,mesh->vertices_ids);
        geometries_fill_index_buffer(fbx_mesh->faces,fbx_mesh->vertex_normal.indices,mesh->normals_ids);
        geometries_fill_index_buffer(fbx_mesh->faces,fbx_mesh->vertex_uv.indices,mesh->uvs_ids);


        memset(mesh->material_ids,0,material_ids_size);

        for(uint mat_i = 0; mat_i<fbx_mesh->materials.count;mat_i++)
        {
            ufbx_mesh_material mesh_mat = fbx_mesh->materials.data[mat_i];
            for(uint i = 0; i<mesh_mat.face_indices.count;i++)
            {
                uint face_i =mesh_mat.face_indices.data[i];
                mesh->material_ids[face_i] = mesh_mat.material->element.typed_id;
            }
        }


        for(uint i = 0; i<mesh->vertices_count;i++)
        {
            for(uint j = 0; j<3;j++)
                mesh->vertices[i][j] = fbx_mesh->vertices.data[i].v[j];
        }
        for(uint i = 0; i<mesh->normal_count;i++)
        {
            for(uint j = 0; j<3;j++)
            {
                mesh->normals[i][j] = fbx_mesh->vertex_normal.values.data[i].v[j];
            }
        }

        for(uint i =0;i<mesh->uvs_count;i++)
        {
            for(uint j=0;j<2;j++)
            {
                mesh->uvs[i][j] = fbx_mesh->vertex_uv.values.data[i].v[j];
            }
        }

        mesh->bvh_tree = bvhtree_triangles_build(arena,mesh);

        for(uint i = 0; i<fbx_mesh->instances.count;i++)
        {
            
            geoms.instances[instance_i].mesh_id = mesh_i;
            geoms.instances[instance_i].type = JPC_TRIANGLE;

            ufbx_matrix ufbx_mat =  fbx_mesh->instances.data[i]->node_to_world;
            
            ufbx_transform_to_jpc_transform(
                &ufbx_mat,geoms.instances[instance_i].transformations               
            );




            instance_i++;
        }
    }

    geoms.bvhtree_instances = bvhtree_instances_build(arena,&geoms);

    return geoms;
}


materiallib_t material_load_fbx(arena_t* arena, ufbx_material_list fbx_materials)
{


    uint created_materials = fbx_materials.count;
    if(created_materials==0)
    {
        created_materials=1;
    }

    materiallib_t matlib = {
        .materials = NULL,
        .materials_n = created_materials,
        .textures = NULL,
        .textures_count = 0,
    };

    shaders_t shaders_lib = shaders_init();
    shaders_load_defaults(shaders_lib);
    assert(strcmp(shaders_lib.shaders[0].name,"Diffuse")==0); 


    shader_t* shaders = malloc(sizeof(shader_t)* created_materials);
    for(uint i =0;i<created_materials;i++)
    {
        shaders[i] = shaders_lib.shaders[0];
    }

    matlib.materials = materials_init(arena,shaders,created_materials) ;

    for(int i = 0; i<fbx_materials.count;i++)
    {
        float4 color;
        for(uint j=0;j<4;j++)
        {
            color[j] = fbx_materials.data[i]->pbr.base_color.value_vec4.v[j];
        }

        material_set_uniform(matlib.materials+i,shaders+i,0,color);
    }
    return matlib;
}

camera_t camera_load_fbx(arena_t* arena, ufbx_camera* fbx_camera)
{
    //transform
    ufbx_matrix t = fbx_camera->element.instances.data[0]->node_to_world;

    camera_t camera = {
        .clip_end = fbx_camera->far_plane,
    };
    if(fbx_camera->projection_plane.x>fbx_camera->projection_plane.y)
    {
        camera.near_plane = 1./fbx_camera->projection_plane.x;
    }
    else{
        camera.near_plane = 1./fbx_camera->projection_plane.y;
    }
    ufbx_transform_to_jpc_transform(&t,camera.transformation);

    mat4 temp,res;
    glm_mat4_ucopy(camera.transformation,temp);

    mat4 inverse = {
        {0,0,1,0},
        {0,1,0,0},
        {-1,0,0,0},
        {0,0,0,1}
    };

    glm_mat4_mul(temp,inverse,res);

    glm_mat4_ucopy(res,camera.transformation);

    
    return camera;
}

scene_t* scene_load_fbx(arena_t* arena, const char* path, char** error_msg)
{
    ufbx_load_opts opts = {0}; // Optional, pass NULL for defaults
    
    ufbx_error     error; // Optional, pass NULL if you don't care about errors
    *error_msg = NULL;
    ufbx_scene* fbx_scene = ufbx_load_file(path, &opts, &error);
    if (!fbx_scene)
    {
        size_t str_length = strlen(error.description.data);
        *error_msg = arena_alloc(arena, str_length+1);
        strcpy(*error_msg, error.description.data);
        return NULL;
    }


    scene_t* scene = arena_alloc(arena,sizeof(scene_t));

    scene->lights = lights_load_fbx(arena,fbx_scene->lights);
    scene->camera = camera_load_fbx(arena,fbx_scene->cameras.data[0]);
    scene->geometries = geometries_load_fbx(arena,fbx_scene->meshes);
    scene->materiallib = material_load_fbx(arena,fbx_scene->materials);
    return scene;
}