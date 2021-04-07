#include "ShadePrograms.h"
#include <cstddef>



namespace jpctracer::raytracing
{
    ShadePrograms::ShadePrograms(const void** materials, TracingContext* context,UserThreadData* user_data) 
        : m_materials(materials),m_context(context),m_user_data(user_data)
    {
        
    }
    
    void ShadePrograms::ExecuteClosestHit(const SurfaceInteraction& interaction) 
    {
        int mat_id = interaction.MaterialId;
        const void* material = m_materials[mat_id];
        m_closest_hit_per_mat[mat_id](material,m_user_data,m_ray,interaction,m_payload,m_context);
    }
    
    AnyHitResult ShadePrograms::ExecuteAnyHit(const SurfaceInteraction& interaction) 
    {
        int mat_id = interaction.MaterialId;
        const void* material = m_materials[mat_id];
        return m_any_hit_per_mat[mat_id](material,m_user_data,m_ray,interaction,m_payload);
    }
    
    void ShadePrograms::ExecuteMiss() 
    {
        m_mis_func(m_user_data,m_ray,m_payload);
    }



    void ShadePrograms::SetRayType(const Ray& ray, MissFuncPtr mis_func,const AnyHitFuncPtr* any_hit_per_mat,
        const ClosestHitFuncPtr* closest_hit_per_mat, Payload* payload ) 
    {
        m_mis_func = mis_func;
        m_any_hit_per_mat = any_hit_per_mat;
        m_closest_hit_per_mat = closest_hit_per_mat;
        m_payload = payload;
        m_ray = ray;
    }
    
    void UpdateShadePrograms(const Scene& scene,const Ray& ray,Payload* payload, ShadePrograms& programs) 
    {
        const AnyHitFuncPtr* any_hit_per_mat = &scene.any_hit_programs[ray.Type*scene.ray_types_count];
        const ClosestHitFuncPtr* closest_hit_per_mat = &scene.closest_hit_programs[ray.Type*scene.ray_types_count];
        programs.SetRayType(ray, scene.mis_programs[ray.Type],any_hit_per_mat ,closest_hit_per_mat,payload);
    }

}