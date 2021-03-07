#include "ShadingData.h"
#include "jpc_tracer/raytracing/Base.h"
#include "jpc_tracer/raytracing/Scene.h"



namespace jpctracer::raytracing
{
    void UpdateShadePrograms(const ShadingData& data,const Ray& ray, Payload* payload, ShadePrograms* programs) 
    {
        int id = ray.Type;
        programs->NewRay(ray, payload, data.any_hit_funs.GetRow(id), data.closest_hit_funs.GetRow(id), data.miss_funcs.Get(id), 
                data.material_inits.GetRow(id), data.material_settings.GetRow(id), data.material_alloc_info.GetRow(id));
            
    }
}