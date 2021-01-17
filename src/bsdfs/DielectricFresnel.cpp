#include "DielectricFresnel.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include <cmath>



namespace bsdfs
{
    DielectricFresnel::DielectricFresnel(core::Prec index_of_refraction_inner,core::Prec index_of_refraction_outer) 
        : _index_of_refraction_inner(index_of_refraction_inner), _index_of_refraction_outer(index_of_refraction_outer)
    {
        
    }

    core::Spectrum DielectricFresnel::Evaluate(core::Prec cos_theta_incoming) const 
    {
        /*
        core::Prec eta_t = _index_of_refraction_inner;
        core::Prec eta_i = _index_of_refraction_outer;

        std::swap(eta_t,eta_i);

        bool entering = cos_theta_incoming > 0.f;
        if(! entering)
        {
            std::swap(eta_t,eta_i);
            cos_theta_incoming = std::abs(cos_theta_incoming);
        }

        core::Prec eta_t2 = eta_t*eta_t;
        core::Prec eta_i2 = eta_i*eta_i;

        core::Prec cos_theta_outer = cos_theta_incoming;
        core::Prec cos_theta_inner = std::sqrt(std::max(0.f,eta_i2+eta_t2*cos_theta_outer*cos_theta_outer - eta_t2)/ eta_i2);

        core::Prec etacos_ti = eta_t * cos_theta_outer;
        core::Prec etacos_it = eta_i * cos_theta_inner;
        core::Prec etacos_ii = eta_i * cos_theta_outer;
        core::Prec etacos_tt = eta_t * cos_theta_inner;


        core::Prec r_parallel = (etacos_ti-etacos_it) / (etacos_ti + etacos_it);
        core::Prec r_perpendicular = (etacos_ii-etacos_tt) / (etacos_ii+etacos_tt);

        core::Prec fresnel = (r_parallel*r_parallel+r_perpendicular*r_perpendicular) / 2;

        return core::Spectrum::FromValue(1)*fresnel;*/

        //See Microfacet Models for Refraction through Rough Surfaces Section 5.1

        core::Prec eta_t = _index_of_refraction_inner;
        core::Prec eta_i = _index_of_refraction_outer;

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