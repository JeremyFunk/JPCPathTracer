#include <integrator.h>
#include <jpc_api.h>
#include <jpc_prefabs.h>

int main()
{
    scene_manager_t* manager = scene_manager_init();

    float quad1_verts[4][3] = {
        {0, 0, 0},
        {0, 1, 0},
        {1, 1, 0},
        {1, 0, 0},
    };

    float quad2_verts[4][3] = {
        {0, 0, 0},
        {1, 0, 0},
        {1, 0, 1},
        {0, 0, 1},
    };

    instance_handle_t quad1
        = scene_manager_create_quad(manager, quad1_verts, MAT_MIRROR1);

    instance_handle_t quad2
        = scene_manager_create_quad(manager, quad2_verts, MAT_DIFFUSE1);

    float4 color = {1, 1, 1, 1};

    scene_manager_material_set_uniform(manager, MAT_DIFFUSE1, "color", color);

    point_light_t light = {
        .color = {1.,1.,1.,1.},
        .falloff = 1.,
        .position = {1,1,1},
        .strength = 10,
    };

    scene_manager_create_point_light(manager,light);

    scene_t* scene = scene_manager_get_scene(manager);

    sampler_state* sampler = sampler_init();

    integrator_t* integrator = integrator_init(2, scene, sampler, 4);


    vec4 dest;

    vec3 impact_point = {0.5,0.5,0};

    ray_t ray = {
        .clip_end = 400.,
        .origin = {0.5,1.,0.5}
    };

    glm_vec3_sub(impact_point,ray.origin,ray.direction);
    glm_vec3_normalize(ray.direction);




    integrate(integrator,ray,dest);

    printf("Result:");

    for(uint i =0;i<4;i++)
    {
        printf("%f,",dest[i]);
    }
    printf("\n");

    integrator_free(integrator);

    sampler_free(sampler);
    
    scene_manager_free(manager);



}