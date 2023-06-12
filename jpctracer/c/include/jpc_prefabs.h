#include <jpc_api.h>

typedef struct scene_manager_s scene_manager_t;

typedef struct
{
    uint id;

} instance_handle_t;

typedef enum
{
    MAT_DIFFUSE1 = 0,
    MAT_DIFFUSE2,
    MAT_DIFFUSE3,
    MAT_DIFFUSE4,
    MAT_MIRROR1,
    MAT_MIRROR2,
    MAT_MIRROR3,
    MAT_MIRROR4,
    MAT_COUNT,

} material_id_e;

scene_manager_t* scene_manager_init();

void scene_manager_free(scene_manager_t* manager);

instance_handle_t scene_manager_create_quad(scene_manager_t* manager,
                                            float            points[4][3],
                                            material_id_e    material);

instance_handle_t scene_manager_create_sphere(scene_manager_t* manager,
                                              sphere_geometry_t sphere,
                                                  material_id_e material);

// when instances are changed the scene is invalid
scene_t* scene_manager_get_scene(scene_manager_t* manager);

void scene_manager_material_set_uniform(scene_manager_t* manager,
                                        material_id_e    material_id,
                                        const char*      shader_prop,
                                        const float*     value);

void scene_manager_create_point_light(scene_manager_t* manager,
                                      point_light_t    light);