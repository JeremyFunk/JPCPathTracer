#include "Bsdf.h"
#include "bsdfs/SphericalCoordinates.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>
#include "MonteCarlo.h"

namespace jpc_tracer {

    struct BsdfMemory
    {
        BsdfMemory(int max_size, int max_bsdf_count)
            : Pool(max_size), Parameters(max_bsdf_count), Closures(max_bsdf_count), FreeSpace(max_size) , 
            Weights(max_bsdf_count) , MaxBsdfCount(max_bsdf_count)
        {
            FreeBsdfMem = Pool.data();
        }

        std::vector<std::byte> Pool;
        std::vector<void*> Parameters;
        std::vector<const BsdfClosure*> Closures;
        std::vector<Prec> Weights;
        int BsdfCounter = 0;
        void* FreeBsdfMem;
        size_t FreeSpace;
        int MaxBsdfCount;
        Vec3 Normal;
        Vec3 Tangent1;
        Vec3 Tangent2;
    };


    BsdfMemoryPtr CreateBsdfMemory(BsdfMemoryInfo info) 
    {
        return MakeRef<BsdfMemory>(info.max_byte_size,info.max_bsdf_count);
    }
    
    void* MallocBsdf(BsdfMemoryPtr& memory,int size, Prec weight,const BsdfClosure* closure, size_t alignment) 
    {
        
        void* bsdf_memory = std::align(alignment,size,memory->FreeBsdfMem,memory->FreeSpace);

        if(bsdf_memory != nullptr)
        {
            memory->FreeBsdfMem = ((std::byte*)bsdf_memory)+size;
            memory->FreeSpace -= size;
        }
        
        if(memory->MaxBsdfCount <= memory->BsdfCounter)
            return nullptr;
        memory->Parameters[memory->BsdfCounter] = bsdf_memory;
        memory->Closures[memory->BsdfCounter] = closure;
        memory->Weights[memory->BsdfCounter] = weight;

        memory->BsdfCounter++;

        return bsdf_memory;
    }
    
    Vec3 WorldToLocal(const BsdfMemoryPtr& memory,Vec3 x)
    {
        return Vec3({x.dot(memory->Tangent1),x.dot(memory->Tangent2),x.dot(memory->Normal)});
    }
    
    Vec3 LocalToWorld(const BsdfMemoryPtr& memory,Vec3 x)
    {
        Vec3& tangent1 = memory->Tangent1; 
        Vec3& tangent2 = memory->Tangent2; 
        Vec3& normal = memory->Normal;
        return Vec3({
            tangent1[0]*x[0]+tangent2[0]*x[1]+normal[0]*x[2],
            tangent1[1]*x[0]+tangent2[1]*x[1]+normal[1]*x[2],
            tangent1[2]*x[0]+tangent2[2]*x[1]+normal[2]*x[2]
        });
    }

    void ResetBsdfMemory(BsdfMemoryPtr& memory, Vec3 normal) 
    {
        memory->BsdfCounter = 0;
        memory->FreeBsdfMem = memory->Pool.data();
        memory->FreeSpace = memory->Pool.size();
        memory->Normal = normal;

        Vec3 temp_v = {0,0,-1};
        if(normal[2]!=0)
        {
            temp_v = {0,-1,0};
        }
        memory->Tangent1 = normal.cross(temp_v).normalized();
        memory->Tangent2 = normal.cross(memory->Tangent1).normalized();
    }
    
    SpectrumPasses ScatteringBsdf(const BsdfMemoryPtr& memory, const Vec3& scattered_direction,const Vec3& incident_direction) 
    {

        Vec3 scattered_direction_ns = WorldToLocal(memory, scattered_direction);
        Vec3 incident_direction_ns = WorldToLocal(memory, incident_direction);
        SpectrumPasses luminance = SpectrumPasses::FromValue(0);
        for(int bsdf_idx = 0; bsdf_idx< memory->BsdfCounter; bsdf_idx++)
        {
            void* parameter = memory->Parameters[bsdf_idx];
            Prec weight = memory->Weights[bsdf_idx];
            luminance += memory->Closures[bsdf_idx]->_Scattering(parameter, scattered_direction_ns, incident_direction_ns) * weight;

        }

        return luminance;
    }
    
    Prec PdfBsdf(const BsdfMemoryPtr& memory, const Vec3& scattered_direction,const Vec3& incident_direction) 
    {
        Vec3 scattered_direction_ns = WorldToLocal(memory, scattered_direction);
        Vec3 incident_direction_ns = WorldToLocal(memory, incident_direction);
        Prec pdf = 0;
        //already the balance heuristic
        for(int bsdf_idx = 0; bsdf_idx< memory->BsdfCounter; bsdf_idx++)
        {
            void* parameter = memory->Parameters[bsdf_idx];
            Prec weight = memory->Weights[bsdf_idx];
            Prec _pdf = memory->Closures[bsdf_idx]->_Pdf(parameter, scattered_direction_ns, incident_direction_ns);
            pdf += _pdf * weight;
        

        } 
        
        return pdf;
    }
    
    Vec3 SampleIncidentDirectionBsdf(const BsdfMemoryPtr& memory, const Vec3& scattered_direction, const Vec2& random_point) 
    {
        
        Vec3 scattered_direction_ns = WorldToLocal(memory, scattered_direction.normalized());
        
        auto[bxdf_idx, new_rand_p] = SampleDiscreteDistribution(random_point,memory->Weights);

        
        void* parameter = memory->Parameters[bxdf_idx];

        Vec3 incident_direction_ns = memory->Closures[bxdf_idx]->_SampleIncidentDirection(parameter, scattered_direction_ns, new_rand_p);
        
        auto result = LocalToWorld(memory, incident_direction_ns);
        
        return result.normalized();
    }



}