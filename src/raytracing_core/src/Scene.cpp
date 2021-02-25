#include "raytracing_core/include/Scene.h"
#include "raytracing_core/include/Geometry.h"
#include "raytracing_core/src/scene/PrivateScene.h"
#include "raytracing_core/src/geometry/PrivateRootGeometry.h"

namespace jpc_rt_core {


    Scene::Scene(void* user_data) 
        : UserData(user_data), m_scene_data(new PrivateScene())
    {
        
    }


    void Scene::AddMaterial(int material_id, int ray_type_id,AllocationData parameter_info, MaterialInitFuncPtr parameter_init ,const void* settings) 
    {
        m_scene_data->Shading.material_inits.Set(ray_type_id, material_id,parameter_init);
        m_scene_data->Shading.material_settings.Set(ray_type_id, material_id, settings);
        m_scene_data->Shading.material_alloc_info.Set(ray_type_id, material_id, parameter_info);
        m_scene_data->Shading.max_material_params_size = std::max<int>(parameter_info.Size,m_scene_data->Shading.max_material_params_size );

    }


    void Scene::AddClosestHitProgramm(int material_id, int ray_type_id, ClosestHitFuncPtr program) 
    {
        m_scene_data->Shading.closest_hit_funs.Set(ray_type_id, material_id, program);
    }

    void Scene::AddAnyHitProgramm(int material_id, int ray_type_id, AnyHitFuncPtr program) 
    {
        m_scene_data->Shading.any_hit_funs.Set(ray_type_id, material_id,program);
    }

    void Scene::MissProgramm(int ray_id, MissFuncPtr program) 
    {
        m_scene_data->Shading.miss_funcs.Set(ray_id, program);
    }

    RootGeometry* Scene::CreateRootGeometry(InstanceBVHType instance_bvh, DynamicBVHType dynamic_bvh, StaticBVHType static_bvh) 
    {
        m_scene_data->GeometryRoot.dynamic_bvh_type = dynamic_bvh;
        m_scene_data->GeometryRoot.instance_bvh_type = instance_bvh;
        m_scene_data->GeometryRoot.static_bvh_type = static_bvh;
        
        return new RootGeometry{&m_scene_data->GeometryRoot};
    }


    Scene::~Scene() 
    {
        free(m_scene_data);
    }


}
