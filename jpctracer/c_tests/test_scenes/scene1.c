#include<jpc_api.h>
#include<jpc_prefabs.h>
#include<cglm/cglm.h>


int main()
{
    /*

            Scene: Top down
                      Cam
          -3  -2  -1   0   1   2   3 X
           |   |   |   |   |   |   |
        -2-            *

        -1-    *   L-------L   *
                   L       L
         0-        L   o   L
                   L       L
         1-        L-------L
                    
         2-

         Z
    */


   scene_manager_t* manager = scene_manager_init();

   point_light_t light = {
    .color = {1,1,1,1},
    .falloff = 1,
    .position = {0,2,0},
    .strength = 10,
   };

    float y = -0.5;

    float quad1_verts[4][3] = {
        {-1,y,-1},
        {1,y,-1},
        {1,y,1},
        {-1,y,1},
    };

    float z = 2;
    float s = 5;
    float quad2_verts[4][3] = {
        {-s,-s,z},
        {s,-s,z},
        {s,s,z},
        {-s,s,z},
    };


    sphere_geometry_t sphere_geom = {
        .position = {0,0,0},
        .radius = 0.5,
    };

    camera_t camera = {
        .clip_end = 200,
        .near_plane = 2,
    };
    {
        mat4 temp = GLM_MAT4_IDENTITY_INIT;

        vec3 translation = {-0.054, 0.07, -2};
        glm_translate(temp, translation);
        glm_rotate_y(temp, glm_rad(180), camera.transformation);
    }
    
    

   scene_manager_create_point_light(manager,light);

   instance_handle_t quad1 = scene_manager_create_quad(manager,quad1_verts,MAT_DIFFUSE1);
   instance_handle_t quad2
       = scene_manager_create_quad(manager, quad2_verts, MAT_DIFFUSE2);
   
   instance_handle_t sphere1 = scene_manager_create_sphere(manager,sphere_geom,MAT_MIRROR1) ;

   vec4 diffuse1_color = {1,1,1,1};
   scene_manager_material_set_uniform(
       manager, MAT_DIFFUSE1, "color", diffuse1_color);

   vec4 diffuse2_color = {0.1, 0.9, 1, 1};
   scene_manager_material_set_uniform(
       manager, MAT_DIFFUSE2, "color", diffuse2_color);


    scene_t* scene = scene_manager_get_scene(manager);
    scene->camera = camera;

    render_settings_t settings = {
        .light_samples = 10,
        .max_depth = 5,
        .passes = JPC_PASS_DEPTH | JPC_PASS_NORMAL,
        .subpixels =    20,
        .tile_size = 1,
    };

    const char* path ="C:\\Users\\thoma\\Desktop\\raytracing\\scene1" ;
    path ="C:\\Users\\chris\\dev\\jpc_tracer\\Results\\scene1" ;
    render_and_save(scene,settings,(uint2){1920,1080},path);

    scene_manager_free(manager);

}