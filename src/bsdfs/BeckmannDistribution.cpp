#include "BeckmannDistribution.h"
#include "core/Linalg.h"
#include "bsdfs/SphericalCoordinates.h"
#include "core/constants.h"
#include <cmath>
#include <corecrt_math.h>


namespace bsdfs
{
    BeckmannDistribution::BeckmannDistribution(core::Prec roughness) 
        : _alpha2(0)
    {
        _alpha2 = RoughnessToAlpha(roughness)* RoughnessToAlpha(roughness);
    }

    core::Prec BeckmannDistribution::Distribution(const core::Vec3& half_direction) const
    {
        const core::Vec3& v = half_direction;
        core::Prec exponent = Tan2Theta(v) / _alpha2;
        core::Prec Cos4Theta = Cos2Theta(v)*Cos2Theta(v);
        return std::exp(-exponent) / (M_PI*_alpha2*Cos4Theta);
    }
    
    core::Prec BeckmannDistribution::ViewedPercentage(const core::Vec3& half_incomeing_direction,const core::Vec3& outgoing_direction ) const
    {
        return 1 / (1+ Lambda(half_incomeing_direction) + Lambda(outgoing_direction));
    }
    

    core::Vec3 BeckmannDistribution::SampleFromDistribution(const core::Vec3& incoming_direction, const core::Vec2 random_point) const
    {

        core::Prec log_sample = std::log(random_point[0]);
        if(std::isinf(log_sample)) log_sample = 0;
        core::Prec tan_2_theta = - _alpha2*log_sample;
        core::Prec phi = random_point[1] * 2* M_PI;
        core::Prec cos_theta = 1 / std::sqrt(1+ tan_2_theta);
        core::Prec sin_theta = std::sqrt(std::max<core::Prec>(0,1-cos_theta*cos_theta));
        core::Vec3 direction_half = SphericalDirection(sin_theta, cos_theta, phi);
        if(! SameHemiSphereNormalSpace(direction_half, -incoming_direction)) return -direction_half;
        return direction_half;
    }
    
    core::Prec BeckmannDistribution::Pdf(const core::Vec3& incoming_direction,const core::Vec3& half_direction) const
    {
        core::Prec dist_in = Distribution(incoming_direction);
        core::Prec viewed_perc = SingleViewedPercentage(incoming_direction);

        core::Prec cos_theta = AbsCosTheta(incoming_direction);

        return dist_in * viewed_perc
        * std::abs(incoming_direction.dot(half_direction)) / cos_theta;
    }
    
    core::Prec BeckmannDistribution::SingleViewedPercentage(const core::Vec3& direction) const
    {
        return 1 / (1+Lambda(direction));
    }
    
    core::Prec BeckmannDistribution::RoughnessToAlpha(core::Prec roughness) const
    {
        roughness = std::max(roughness, (core::Prec)1e-3);
        core::Prec x = std::log(roughness);
        return 1.62142f + 0.819955f * x + 0.1734f * x * x +
               0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
    }
    
    core::Prec BeckmannDistribution::Lambda(const core::Vec3& v) const 
    {
        //With approx of the exp and erf
        core::Prec absTanTheta = std::abs(TanTheta(v));
        if (std::isinf(absTanTheta)) return 0.;
        core::Prec a = 1 / (sqrt(_alpha2) * absTanTheta);
        if (a >= 1.6f) return 0;
        return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
    }
}