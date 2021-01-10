#pragma once
#include "bsdfs/IBXDF.h"
#include "core/IBSDF.h"
#include "core/Linalg.h"
#include <algorithm>
#include <vector>

namespace bsdfs {
    
    class BSDF: public core::IBSDF
    {
    public:
        
        BSDF(IBXDF* bxdfs, int bxdfs_count, const core::Vec3& normal);

        virtual core::BSDFResult EvaluateAll(const core::Vec3& scattered_direction,const core::Vec2& random_point,core::BxDFType type = core::BxDFType()) const override;
        virtual core::SpectrumPasses Scattering(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const override;
        virtual core::Prec Pdf(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const override;
        virtual core::Vec3 SampleIncidentDirection(const core::Vec3& scattered_direction, const core::Vec2& random_point) const override;

        ~BSDF();
        
    private:
        core::Vec3 WorldToLocal(core::Vec3 x) const;
        core::Vec3 LocalToWorld(core::Vec3 x) const;
        void SetNormal(const core::Vec3& normal);
        const IBXDF* _bxdfs;
        int _bxdfs_count;
        core::Vec3 _tangent1;
        core::Vec3 _tangent2;
        core::Vec3 _normal;

    };
}