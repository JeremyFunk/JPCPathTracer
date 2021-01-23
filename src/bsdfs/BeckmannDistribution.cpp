#include "BeckmannDistribution.h"
#include "core/Linalg.h"
#include "SphericalCoordinates.h"
#include <cmath>


namespace jpc_tracer
{
    Prec BeckmannDistribution::Distribution(const BeckmannParams* params,const Vec3& microfacet_normal) const
    {
        Prec cos2_theta = Cos2Theta(microfacet_normal);
        Prec cos4_theta = cos2_theta*cos2_theta;
        Prec tan2_theta = Tan2Theta(cos2_theta);


        Prec alpha2 = params->Alpha*params->Alpha;

        Prec exponent = std::exp(- tan2_theta / alpha2);

        Prec d = 1.f / (M_PI * alpha2*cos4_theta) * exponent;
        return d;
    }
    
    Prec BeckmannDistribution::ShadowMasking(const BeckmannParams* params,const Vec3& scattered_direction,const Vec3& incident_direction) const
    {
        return 1 / (1+Lambda(params, scattered_direction) + Lambda(params, incident_direction));
    }
    
    Vec3 BeckmannDistribution::SampleFromDistribution(const BeckmannParams* params, const Vec2 random_point) const
    {
        Prec log_sample = std::log(random_point[0]);
        if(std::isinf(log_sample))
            log_sample = 0;
        
        Prec alpha2 = params->Alpha*params->Alpha;

        Prec tan2_theta = -alpha2 * log_sample;
        Prec phi = random_point[1] * 2 *M_PI;
        Prec cos_theta = 1 / std::sqrt(1+tan2_theta);
        Prec sin_theta = std::sqrt(std::max(0.f,1-cos_theta*cos_theta));
        Prec x = sin_theta*std::cos(phi);
        Prec y = sin_theta*std::sin(phi);
        Prec z = cos_theta;

        return {x,y,z};

    }
    
    Prec BeckmannDistribution::Pdf(const BeckmannParams* params,const Vec3& scattered_direction,const Vec3& micorfacet_normal) const
    {
        Prec pdf = Distribution(params, micorfacet_normal)*std::abs(CosTheta(micorfacet_normal));
        return pdf;
    }
    
    Prec BeckmannDistribution::Lambda(const BeckmannParams* params,const Vec3& direction) const
    {
        Prec tan_theta = TanTheta(direction);
        if(std::isinf(tan_theta))
            return 0;
        Prec a = 1 / (tan_theta*params->Alpha);
        if(a>=1.6)
            return 0;
        
        return (1-1.259f * a + 0.396f * a * a) / (3.525f * a + 2.181f *a*a);
    }
}