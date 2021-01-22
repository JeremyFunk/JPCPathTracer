#include "DielectricFresnel.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include <cmath>



namespace bsdfs
{


    core::Spectrum DielectricFresnel::Evaluate(const DielectricFresnelParams* params, core::Prec cos_theta_incoming) const 
    {

        //See Microfacet Models for Refraction through Rough Surfaces Section 5.1

        core::Prec eta_t = params->IndexOfRefractionInner;
        core::Prec eta_i = params->IndexOfRefractionOuter;

        core::Prec c = cos_theta_incoming;

        core::Prec g2 = (eta_t*eta_t) / (eta_i*eta_i) -1 + c*c;
        core::Prec fresnel;
        if (g2 < 0)
            fresnel = 1;
        else
        {
            core::Prec g = std::sqrt(g2);
            fresnel = 0.5 * std::pow((g - c) / (g + c), 2) * (1.f + std::pow((c * (g + c) - 1.f) / (c * (g - c) + 1.f), 2));
        }


        return core::Spectrum::FromValue(1)*(1-fresnel);

    }
}