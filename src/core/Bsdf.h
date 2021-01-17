#pragma once
#include "Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include <memory>
namespace core {
    
    struct BsdfResult
    {
        Spectrum Scattering;
        Prec Pdf;
        Vec3 IncidentDirection;
    };
    

    struct BsdfClosure
    {
        virtual BsdfResult _EvaluateAll(const void* params, const Vec3& scattered_direction,const Vec2& random_point) const = 0;
        virtual Spectrum _Scattering(const void* params, const Vec3& scattered_direction,const Vec3& incident_direction) const = 0;
        virtual Prec _Pdf(const void* params, const Vec3& scattered_direction,const Vec3& incident_direction) const = 0;
        virtual Vec3 _SampleIncidentDirection(const void* params, const Vec3& scattered_direction, const Vec2& random_point) const = 0;
    };

    struct BsdfMemoryInfo
    {
        int max_byte_size, max_bsdf_count;
    };

    struct BsdfMemory;

    using BsdfMemoryPtr = std::shared_ptr<BsdfMemory>;

    BsdfMemoryPtr CreateBsdfMemory(BsdfMemoryInfo info);

    //returns the bsdf parameter memory
    void* MallocBsdf(BsdfMemoryPtr& memory,int size, Prec weight,const BsdfClosure* callback);

    BsdfMemoryPtr CloneBsdf(const BsdfMemoryPtr& memory);

    BsdfResult EvaluateAllBsdf(const BsdfMemoryPtr& memory, const Vec3& scattered_direction,const Vec2& random_point);

    SpectrumPasses ScatteringBsdf(const BsdfMemoryPtr& memory, const Vec3& scattered_direction,const Vec3& incident_direction);

    Prec PdfBsdf(const BsdfMemoryPtr& memory, const Vec3& scattered_direction,const Vec3& incident_direction);

    Vec3 SampleIncidentDirectionBsdf(const BsdfMemoryPtr& memory, const Vec3& scattered_direction, const Vec2& random_point);






}