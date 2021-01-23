#pragma once
#include"Base.h"
#include "Bsdf.h"
#include <iostream>
#include <memory>
#include <utility>


namespace jpc_tracer {

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
            return EvaluateAll((const ParamsT*)params,scattered_direction,random_point);
        }
        Spectrum _Scattering(const void* params, const Vec3& scattered_direction,const Vec3& incident_direction) const final
        {
            return Scattering((const ParamsT*)params,scattered_direction,incident_direction);
        }
        Prec _Pdf(const void* params, const Vec3& scattered_direction,const Vec3& incident_direction) const final
        {
            return Pdf((const ParamsT*)params, scattered_direction, incident_direction);
        }
        Vec3 _SampleIncidentDirection(const void* params, const Vec3& scattered_direction, const Vec2& random_point) const final
        {
            return SampleIncidentDirection((const ParamsT*)params, scattered_direction, random_point);
        }
        virtual ~BsdfClosureGeneric() = 0;
    };


    template<class ParamsT>
    BsdfClosureGeneric<ParamsT>::~BsdfClosureGeneric()
    {
        
    }
    



    template<class ParamsT>
    class BsdfGeneric
    {
    public:
        BsdfGeneric(Ref<BsdfClosureGeneric<ParamsT>> closure) 
            : _closure(closure) {
            }
        
        ParamsT* Setup(BsdfMemoryPtr& memory, Prec weight) const
        {
            const BsdfClosure* closure = _closure.get();
            return (ParamsT*) MallocBsdf(memory,sizeof(ParamsT), weight,closure,alignof(ParamsT));
        }

        size_t GetMaxSize() const
        {
            return sizeof(ParamsT);
        }



    private:
        Ref<const BsdfClosureGeneric<ParamsT>> _closure;
    };
}