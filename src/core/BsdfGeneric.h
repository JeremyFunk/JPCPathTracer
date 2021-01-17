#pragma once
#include "Bsdf.h"
#include <memory>

namespace core {

    template<class ParamsT>
    class BsdfClosureGeneric : public BsdfClosure
    {
    public:

        virtual BsdfResult EvaluateAll(const ParamsT* params, const Vec3& scattered_direction,const Vec2& random_point) const = 0;
        virtual Spectrum Scattering(const ParamsT* params, const Vec3& scattered_direction,const Vec3& incident_direction) const = 0;
        virtual Prec Pdf(const ParamsT* params, const Vec3& scattered_direction,const Vec3& incident_direction) const = 0;
        virtual Vec3 SampleIncidentDirection(const ParamsT* params, const Vec3& scattered_direction, const Vec2& random_point) const = 0;


        BsdfResult _EvaluateAll(const void* params, const Vec3& scattered_direction,const Vec2& random_point) const final
        {
            return EvaluateAll(params,scattered_direction,random_point);
        }
        Spectrum _Scattering(const void* params, const Vec3& scattered_direction,const Vec3& incident_direction) const final
        {
            return Scattering(params,scattered_direction,incident_direction);
        }
        Prec _Pdf(const void* params, const Vec3& scattered_direction,const Vec3& incident_direction) const final
        {
            return Pdf(params, scattered_direction, incident_direction);
        }
        Vec3 _SampleIncidentDirection(const void* params, const Vec3& scattered_direction, const Vec2& random_point) const final
        {
            return SampleIncidentDirection(params, scattered_direction, random_point);
        }
    };

    template<class ParamsT>
    class BsdfGeneric
    {
    public:
        BsdfGeneric(std::unique_ptr<BsdfClosureGeneric<ParamsT>>&& closure) 
            : _closure(closure) {}
        
        ParamsT* Setup(BsdfMemoryPtr& memory, Prec weight)
        {
            return MallocBsdf(memory,sizeof(ParamsT), weight,_closure.get());
        }
    private:
        std::unique_ptr<BsdfClosureGeneric<ParamsT>> _closure;
    };
}