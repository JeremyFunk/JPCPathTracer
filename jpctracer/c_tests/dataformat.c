#include<jpc_api.h>
#include<cglm/cglm.h>

#define UNIFORM_MAX 5
#define MAX_MATERIALS 5


#define COUNT_OF(x)                                                            \
    ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

typedef struct{
    const char* key;
    float val[4];
    const char* texture;
} uniform_pair;

typedef struct 
{
    const char* name;
    const char* shader;
    uniform_pair uniforms[UNIFORM_MAX];

} parse_material;


typedef struct{
    float position[3];
    float rotation[3];
    float near_plane;
    float clip_end;
} parse_camera;

typedef struct{
    float position[3];
    float rotation[3];
    float scale[3];
    
    geometry_type_t type;
    const char* obj_path;
    float radius;

    const char* materials[MAX_MATERIALS];

} parse_object;

scene_t create_scene(parse_material* materials, uint materials_count,
                    sun_light_t* sunlights, uint sunlights_count,
                    point_light_t* pointlights, uint pointlights_count,
                    parse_object* objects, uint objects_count,
                    parse_camera camera
                    )
{

}

int main()
{
    parse_material materials[] = {
        {
            .name = "Material1",
            .shader = "Diffuse",
            .uniforms = {
                {"color",{1,1,1,1} },
                {"roughness",{0.1}}
            }
            
        },
        {
            .name = "Material2",
            .shader = "Mirror",
            
        }
    };

    sun_light_t sun_lights[] ={
        {
            .color = {1.,1.,2.,3},
            .strength = 2.,
        }
    };

    point_light_t point_lights[] = {
        {
            .color = {1.,2.,3.,1.},
            .position = {4,1,6},
            .falloff = 1,
            .strength = 2,
        }
    };

    parse_camera camera = {
        .position = {-7.8,-6.3,5.15},
        .rotation = {62,0,-51},
    };

    parse_object objects[] = {
        {
            .position = {0,2,3},
            .rotation = {90,180,90},
            .scale = {1,1.,1.},
            .type = JPC_SPHERE,
            .radius = 2.,
            .materials = {
                "Material1"
            }
        },
        {
            .position = {0,1,2},
            .type = JPC_TRIANGLE,
            .obj_path = "",
            .materials = {
                "Material1",
                "Material2",
            },
        },
    };



    scene_t scene = create_scene(
        materials, COUNT_OF(materials),
        sun_lights,COUNT_OF(sun_lights),
        point_lights,COUNT_OF(point_lights),
        objects, COUNT_OF(objects),
        camera
    );

    render_settings_t settings = {
        
    };

    render_and_save(&scene,settings,{20,30},"");
    


}