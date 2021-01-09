#include "BSDF.h"
#include "bsdfs/IBXDF.h"
#include "core/Linalg.h"
#include "core/SpectrumPasses.h"
#include <iostream>
namespace bsdfs {
    BSDF::BSDF(IBXDF* bxdfs,int bxdfs_count,const core::Vec3& normal)
        : Bxdfs(bxdfs), BxdfsCount(bxdfs_count)
    {
        SetNormal(normal);
    }
    
    BSDF::~BSDF() 
    {
        for(int i = BxdfsCount - 1; i >= 0; i--)
            delete &Bxdfs[i];
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
    
    core::DirectionInfo BSDF::Evaluate(const core::Vec3& incoming_direction,const core::Vec2& random_point,
                core::BxDFType type) 
    {
        core::Vec3 incoming_direction_ns = WorldToLocal(incoming_direction);
        // if (incoming_direction_ns[2] < 0) {
        //     incoming_direction_ns = -incoming_direction_ns;
        // }
        core::DirectionInfo result;
        BXDFDirectionInfo dir_info =  Bxdfs[0].Evaluate(incoming_direction_ns, random_point);
        result.OutgoingDirection = dir_info.OutgoingDirection;
        result.Pdf = dir_info.Pdf;
        result.SurfaceColor = core::SpectrumPasses(dir_info.SurfaceColor);
        //result.OutgoingDirection = incoming_direction;
        //result.OutgoingDirection= {0,0,1};
        if (result.OutgoingDirection[2] < 0)
           result.OutgoingDirection = -result.OutgoingDirection;
        // result.OutgoingDirection = -result.OutgoingDirection;
        result.OutgoingDirection = LocalToWorld(result.OutgoingDirection);
        //result.SurfaceColor = core::SpectrumPasses(core::Spectrum::FromRGB({ 1,1,1 }));
        core::Vec3 reflection_ray = incoming_direction - 2 * _normal.dot(incoming_direction) * _normal;
        //result.OutgoingDirection = reflection_ray;
        //std::cout << result.OutgoingDirection[0] << std::endl;
        return result;
    }
    
    core::Vec3 BSDF::WorldToLocal(core::Vec3 x) 
    {
        return core::Vec3({x.dot(_tangent1),x.dot(_tangent2),x.dot(_normal)});
    }
    
    core::Vec3 BSDF::LocalToWorld(core::Vec3 x) 
    {
        return core::Vec3({
            _tangent1[0]*x[0]+_tangent2[0]*x[1]+_normal[0]*x[2],
            _tangent1[1]*x[0]+_tangent2[1]*x[1]+_normal[1]*x[2],
            _tangent1[2]*x[0]+_tangent2[2]*x[1]+_normal[2]*x[2]
        });
    }





}