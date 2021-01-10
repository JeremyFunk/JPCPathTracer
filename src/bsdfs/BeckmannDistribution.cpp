#include "BeckmannDistribution.h"
#include "bsdfs/IMicrofacetDistribution.h"
#include "SphericalCoordinates.h"
#include "core/Linalg.h"
#include "core/constants.h"
#include "MicrofacetHelper.h"

namespace bsdfs
{
    BeckmannDistribution::BeckmannDistribution(core::Prec alpha) 
    {
        _alpha = alpha;
    }

    core::Prec BeckmannDistribution::Distribution(const core::Vec3& microfacet_normal) const 
    {
        if(microfacet_normal[2] <0)
        {
            return 0;
        }
        //macro normal is the surface normal
        core::Prec cos_micro_macro_theta = microfacet_normal[2];

        core::Prec lower_fraction = M_PI*_alpha*_alpha*std::pow(cos_micro_macro_theta,4);

        core::Prec tan_2_alpha2 = Tan2Theta(microfacet_normal) / (_alpha*_alpha);

        core::Prec result = std::exp(-tan_2_alpha2 ) / lower_fraction;
        return result; 
    }
    
    core::Prec BeckmannDistribution::ShadowMasking(const core::Vec3& scattered_direction,const core::Vec3& incident_direction) const 
    {
        return 1 / (1 + Lambda(scattered_direction) + Lambda(incident_direction));
    }
    
    core::Vec3 BeckmannDistribution::SampleFromDistribution(const core::Vec3& incoming_direction, const core::Vec2 random_point) const 
    {
        return core::Vec3();
    }
    
    core::Prec BeckmannDistribution::Pdf(const core::Vec3& incoming_direction,const core::Vec3& micorfacet_normal) const 
    {
        return 1;
    }
    
    core::Prec BeckmannDistribution::Lambda(const core::Vec3& x) const
    {
        core::Prec absTanTheta = std::abs(TanTheta(x));
        if (std::isinf(absTanTheta)) return 0.;
        // Compute _alpha_ for direction _w_
        core::Prec a = 1 / (_alpha * absTanTheta);
        if (a >= 1.6f) return 0;
        return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
    }
    core::Prec BeckmannDistribution::RoughnessToAlpha(core::Prec roughness) {
        roughness = std::max(roughness, (core::Prec)1e-3);
        core::Prec x = std::log(roughness);
        return 1.62142f + 0.819955f * x + 0.1734f * x * x +
               0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
    }
    

}