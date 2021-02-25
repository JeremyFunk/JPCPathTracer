#include "ShadingData.h"
#include "raytracing_core/include/Base.h"
#include "raytracing_core/include/Scene.h"



namespace jpc_rt_core
{
    void UpdateShadePrograms(const ShadingData& data,const Ray& ray, Payload* payload, ShadePrograms* programs) 
    {
        int id = ray.Type;
        programs->NewRay(ray, payload, data.any_hit_funs.GetRow(id), data.closest_hit_funs.GetRow(id), data.miss_funcs.Get(id), 
                data.material_inits.GetRow(id), data.material_settings.GetRow(id), data.material_alloc_info.GetRow(id));
            
    }
}