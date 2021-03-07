#pragma once
#include "ShadePrograms.h"
#include "jpc_tracer/raytracing/Base.h"
#include "jpc_tracer/raytracing/Scene.h"
#include <vector>

namespace jpctracer::raytracing
{

    template<class T>
    class RayMaterialTable
    {
    public:
        RayMaterialTable(const T& default_val) : m_default_val(default_val) {}
        
        void Set(int ray_id, int material_id, const T& data)
        {
            while(ray_id>m_data.size()) m_data.push_back(std::vector<T>());
            while(material_id>m_data[ray_id].size()) m_data[ray_id].push_back(m_default_val);
            m_data[ray_id][material_id] = data;
        }

        const T* GetRow(int ray_id) const
        {
            return m_data[ray_id].data();
        }

    private:
        std::vector<std::vector<T>> m_data;
        T m_default_val;
    };

    template<class T, T DefaultVal>
    class RayTable
    {
    public:
        void Set(int ray_id, T data)
        {
            while(ray_id>m_data.size()) m_data.push_back(DefaultVal);
            m_data[ray_id] = data;
        }
        const T Get(int ray_id) const
        {
            return m_data[ray_id];
        }

    private:
        std::vector<T> m_data;
    };


    struct ShadingData
    {
        RayMaterialTable<AnyHitFuncPtr> any_hit_funs{nullptr};
        RayMaterialTable<ClosestHitFuncPtr> closest_hit_funs{nullptr};
        RayTable<MissFuncPtr, nullptr> miss_funcs;

        RayMaterialTable<MaterialInitFuncPtr> material_inits{nullptr};
        RayMaterialTable<const void*> material_settings{nullptr};
        RayMaterialTable<AllocationData> material_alloc_info{AllocationData{0,0}};
        int max_material_params_size = -1;

    };

    void UpdateShadePrograms(const ShadingData& data,const Ray& ray, Payload* payload, ShadePrograms* programs);
}