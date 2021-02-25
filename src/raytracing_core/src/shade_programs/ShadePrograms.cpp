#include "ShadePrograms.h"
#include "raytracing_core/include/Scene.h"
#include <cstddef>



namespace jpc_rt_core
{
    ShadePrograms::ShadePrograms(const Scene* scene, size_t max_stack_size, int max_material_params_size) 
    : m_scene(scene) ,m_material_params(max_stack_size, max_material_params_size) , m_ray(Ray())
    {
        
    }

    void ShadePrograms::ExecuteClosestHit(const SurfaceInteraction& interaction) 
    {
        int material_id = interaction.MaterialId;
        const void* material_setting = *(m_material_settings+material_id);
        void* params_mem = GetMaterialParams(interaction);
        ClosestHitFuncPtr func = *(m_closesthit_material_funcs+material_id);
        m_material_params.IncrementStack();
        m_last_material_id = -1;
        Ray ray = m_ray;
        func(m_scene,ray, interaction,m_context, m_payload,material_setting, params_mem);
        m_material_params.DecrementStack();
    }
    
    AnyHitResult ShadePrograms::ExecuteAnyHit(const SurfaceInteraction& interaction) 
    {
        int material_id = interaction.MaterialId;
        const void* material_setting = *(m_material_settings+material_id);
        void* params_mem = GetMaterialParams(interaction);
        AnyHitFuncPtr func = *(m_anyhit_material_funcs+material_id);
        
        return func(m_scene,m_ray,interaction,m_payload,material_setting,params_mem);
    }
    
    void ShadePrograms::ExecuteMiss() 
    {
        m_miss_func(m_scene,m_ray,m_payload);
    }
    
    void* ShadePrograms::GetMaterialParams(const SurfaceInteraction& interaction) 
    {
        int material_id = interaction.MaterialId;
        const void* material_setting = *(m_material_settings+material_id);
        if(interaction.MaterialId != m_last_material_id)
        {
            AllocationData alloc_data = *(m_params_infos+material_id);
            m_params_mem = m_material_params.Reallocate(alloc_data);

            MaterialInitFuncPtr material_init = *(m_material_init_params+material_id);
            material_init(m_scene,m_ray,interaction,material_setting, m_params_mem);
        }
        return m_params_mem;
    }
    
    void ShadePrograms::SetContext(TracingContext* context) 
    {
        m_context = context;
    }
    
    void ShadePrograms::NewRay(Ray ray,Payload* payload,const AnyHitFuncPtr* anyhit_material_funcs, const ClosestHitFuncPtr* closesthit_material_funcs, 
            MissFuncPtr miss_func, const MaterialInitFuncPtr* material_init_params, const void* const * material_settings, const AllocationData* params_infos) 
    {
        m_ray = ray;
        m_payload = payload;
        m_anyhit_material_funcs = anyhit_material_funcs;
        m_closesthit_material_funcs = closesthit_material_funcs;
        m_miss_func = miss_func;
        m_material_init_params = material_init_params;
        m_material_settings = material_settings;
        m_params_infos = params_infos;
    }
}