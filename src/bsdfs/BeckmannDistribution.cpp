#include "BeckmannDistribution.h"
#include "core/Linalg.h"
#include "SphericalCoordinates.h"
#include <cmath>


namespace bsdfs
{
    core::Prec BeckmannDistribution::Distribution(const BeckmannParams* params,const core::Vec3& microfacet_normal) const
    {
        core::Prec cos2_theta = Cos2Theta(microfacet_normal);
        core::Prec cos4_theta = cos2_theta*cos2_theta;
        core::Prec tan2_theta = Tan2Theta(cos2_theta);


        core::Prec alpha2 = params->Alpha*params->Alpha;

        core::Prec exponent = std::exp(- tan2_theta / alpha2);

        core::Prec d = 1.f / (M_PI * alpha2*cos4_theta) * exponent;
        return d;
    }
    
    core::Prec BeckmannDistribution::ShadowMasking(const BeckmannParams* params,const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const
    {
        return 1 / (1+Lambda(params, scattered_direction) + Lambda(params, incident_direction));
    }
    
    core::Vec3 BeckmannDistribution::SampleFromDistribution(const BeckmannParams* params, const core::Vec2 random_point) const
    {
        core::Prec log_sample = std::log(random_point[0]);
        if(std::isinf(log_sample))
            log_sample = 0;
        
        core::Prec alpha2 = params->Alpha*params->Alpha;

        core::Prec tan2_theta = -alpha2 * log_sample;
        core::Prec phi = random_point[1] * 2 *M_PI;
        core::Prec cos_theta = 1 / std::sqrt(1+tan2_theta);
        core::Prec sin_theta = std::sqrt(std::max(0.f,1-cos_theta*cos_theta));
        core::Prec x = sin_theta*std::cos(phi);
        core::Prec y = sin_theta*std::sin(phi);
        core::Prec z = cos_theta;

        return {x,y,z};

    }
    
    core::Prec BeckmannDistribution::Pdf(const BeckmannParams* params,const core::Vec3& scattered_direction,const core::Vec3& micorfacet_normal) const
    {
        core::Prec pdf = Distribution(params, micorfacet_normal)*std::abs(CosTheta(micorfacet_normal));
        return pdf;
    }
    
    core::Prec BeckmannDistribution::Lambda(const BeckmannParams* params,const core::Vec3& direction) const
    {
        core::Prec tan_theta = TanTheta(direction);
        if(std::isinf(tan_theta))
            return 0;
        core::Prec a = 1 / (tan_theta*params->Alpha);
        if(a>=1.6)
            return 0;
        
        return (1-1.259f * a + 0.396f * a * a) / (3.525f * a + 2.181f *a*a);
    }
}