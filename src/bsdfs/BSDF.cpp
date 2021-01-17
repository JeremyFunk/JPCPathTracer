#include "BSDF.h"
#include "bsdfs/IBXDF.h"
#include "core/IBSDF.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"
#include <iostream>
namespace bsdfs {
    BSDF::BSDF(IBXDF* bxdfs,int bxdfs_count,const core::Vec3& normal)
        : _bxdfs(bxdfs), _bxdfs_count(bxdfs_count)
    {
        SetNormal(normal);
    }
    
    BSDF::~BSDF() 
    {
        for(int i = _bxdfs_count - 1; i >= 0; i--)
            delete &_bxdfs[i];
    }
    
    void BSDF::SetNormal(const core::Vec3& normal) 
    {
        _normal = normal;
        core::Vec3 temp_v = {0,0,1};
        if(_normal[2]!=0)
        {
            temp_v = {0,1,0};
        }
        _tangent1 = _normal.cross(temp_v);
        _tangent2 = _normal.cross(_tangent1);
        _tangent1.normalize();
        _tangent2.normalize();

    }
    
    core::SpectrumPasses BSDF::Scattering(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const 
    {
        core::Vec3 scat_dir_local = WorldToLocal(scattered_direction);
        core::Vec3 inc_dir_local = WorldToLocal(incident_direction);
        core::SpectrumPasses scattering_result=_bxdfs[0].Scattering(scat_dir_local, inc_dir_local);

        return scattering_result;
    }
    
    core::Prec BSDF::Pdf(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const 
    {
        core::Vec3 scat_dir_local = WorldToLocal(scattered_direction);
        core::Vec3 inc_dir_local = WorldToLocal(incident_direction);
        core::Prec pdf = _bxdfs[0].Pdf(scat_dir_local, inc_dir_local);

        return pdf;
    }
    
    core::Vec3 BSDF::SampleIncidentDirection(const core::Vec3& scattered_direction, const core::Vec2& random_point) const 
    {
        core::Vec3 scat_dir_local = WorldToLocal(scattered_direction);
        core::Vec3 inc_dir_local = _bxdfs[0].SampleIncidentDirection(scat_dir_local, random_point);
        core::Vec3 incident_direction = LocalToWorld(inc_dir_local);
        return incident_direction;
    }
    
    core::BSDFResult BSDF::EvaluateAll(const core::Vec3& scattered_direction,const core::Vec2& random_point,core::BxDFType type ) const
    {
        core::BSDFResult result;
        result.IncidentDirection = SampleIncidentDirection(scattered_direction,random_point);
        result.Pdf = Pdf(scattered_direction, result.IncidentDirection);
        result.Scattering = Scattering(scattered_direction, result.IncidentDirection);
        return result;
    }
        
    
    core::Vec3 BSDF::WorldToLocal(core::Vec3 x) const
    {
        return core::Vec3({x.dot(_tangent1),x.dot(_tangent2),x.dot(_normal)});
    }
    
    core::Vec3 BSDF::LocalToWorld(core::Vec3 x) const
    {
        return core::Vec3({
            _tangent1[0]*x[0]+_tangent2[0]*x[1]+_normal[0]*x[2],
            _tangent1[1]*x[0]+_tangent2[1]*x[1]+_normal[1]*x[2],
            _tangent1[2]*x[0]+_tangent2[2]*x[1]+_normal[2]*x[2]
        });
    }





}