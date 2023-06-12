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

    float4 color = {1, 2, 1, 1};

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


    int render_passes = JPC_PASS_DEPTH | JPC_PASS_NORMAL;

    integrator_t* integrator = integrator_init(4, scene, sampler, 4,render_passes);


    float dest[4+3+1];

    vec3 impact_point = {0.5,0.5,0};

    ray_t ray = {
        .clip_end = 400.,
        .origin = {0.5,1.,0.5}
    };

    glm_vec3_sub(impact_point,ray.origin,ray.direction);
    glm_vec3_normalize(ray.direction);




    integrate(integrator,ray,dest);

    vec4 expected_color_result;
    vec3 quad2_hit_point = {0.5, 0.0, 0.5};
    vec3 diff;

    glm_vec3_sub(light.position, quad2_hit_point,diff);
    float dist2 = glm_vec3_norm2(diff);
    glm_vec3_normalize(diff);
    float w = fabs(ray.direction[2]);
    printf("cos_theta(1) = %f\n", w);
    float w2 = fabs(diff[1]); // normal is {0,-1,0}
    printf("cos_theta(2) = %f\n", w2);
    printf("light source = %f\n", light.strength / dist2);


    glm_vec4_scale(color, light.strength / dist2 * w * w2 /M_PI, expected_color_result);

    printf("expected Color: ");

    for (uint i = 0; i < 4; i++)
    {
        printf("%f,", expected_color_result[i]);
    }
    printf("\n");



    printf("Result:\nColor:");
    for(uint i =0;i<4;i++)
    {
        printf("%f,",dest[i]);
    }
    printf("\n Normal:");
    for(int i = 0; i<3;i++)
    {
        printf("%f,",dest[channel_index(render_passes,JPC_PASS_NORMAL)+i]);
    }
    printf("\n Depth:");
    printf("%f\n",dest[channel_index(render_passes,JPC_PASS_DEPTH)]);

    integrator_free(integrator);

    sampler_free(sampler);
    
    scene_manager_free(manager);



}