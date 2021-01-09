#include "TorranceSparrowBRDF.h"
#include "bsdfs/IBXDF.h"
#include "bsdfs/SphericalCoordinates.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"



namespace bsdfs
{
    TorranceSparrowBRDF::TorranceSparrowBRDF(core::Spectrum basecolor,const IFresnel* fresnel,const IMicrofacetDistribution* distribution) 
        : _fresnel(fresnel), _basecolor(basecolor), _distribution(distribution)
    {
        
    }
    
    core::Spectrum TorranceSparrowBRDF::SurfaceColor(const core::Vec3& incoming_direction_ns,const core::Vec3& outgoing_direction_ns) 
    {
        core::Prec cos_theta_in = AbsCosTheta(incoming_direction_ns);
        core::Prec cos_theta_out = AbsCosTheta(outgoing_direction_ns);
        core::Vec3 half_direction = outgoing_direction_ns + incoming_direction_ns;
        if (cos_theta_out == 0 || cos_theta_in == 0) return core::Spectrum::FromValue(0);
        if (half_direction[0] == 0 && half_direction[1] == 0 && half_direction[2] == 0) return core::Spectrum::FromValue(0);
        half_direction.normalize();
        core::Spectrum F = _fresnel->Evaluate(outgoing_direction_ns.dot(half_direction));
        core::Prec dist = _distribution->Distribution(half_direction);
        core::Prec view = _distribution->ViewedPercentage(half_direction, outgoing_direction_ns);
        core::Prec basecolor_multiplicator = dist * view /
            (4 * cos_theta_in * cos_theta_out);
        core::Spectrum result =  _basecolor * basecolor_multiplicator * F;
        return result;
    }
    


    BXDFDirectionInfo TorranceSparrowBRDF::Evaluate(const core::Vec3& incoming_direction_ns,const core::Vec2& random_point,
            core::BxDFType type)
    {
        BXDFDirectionInfo result;
        
        if(incoming_direction_ns[2]==0)
        {
            result.SurfaceColor = core::Spectrum::FromRGB({ 1,0,0 });
            return result;
        }
        core::Vec3 half_direction = _distribution->SampleFromDistribution(incoming_direction_ns,random_point);

        result.OutgoingDirection =  Reflect(incoming_direction_ns, half_direction);

        if(! SameHemiSphereNormalSpace(incoming_direction_ns,result.OutgoingDirection))
        {
            result.SurfaceColor = core::Spectrum::FromRGB({ 1,0,0 });
            return result;
        }   

        result.Pdf = _distribution->Pdf(incoming_direction_ns,half_direction ) / (4* incoming_direction_ns.dot(half_direction));
        result.SurfaceColor = SurfaceColor(incoming_direction_ns, result.OutgoingDirection);

        return result;
        

    }
}