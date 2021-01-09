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
        ~BSDF();
        void SetNormal(const core::Vec3& normal);
        IBXDF* Bxdfs;
        int BxdfsCount;
        virtual core::DirectionInfo Evaluate(const core::Vec3& incoming_direction,const core::Vec2& random_point,
            core::BxDFType type = core::BxDFType());
    private:
        core::Vec3 WorldToLocal(core::Vec3 x);
        core::Vec3 LocalToWorld(core::Vec3 x);
        
        core::Vec3 _tangent1;
        core::Vec3 _tangent2;
        core::Vec3 _normal;

    };
}