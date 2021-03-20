#include "jpc_tracer/core/core.h"

namespace jpctracer {

    struct test
    {

        std::pair<Spectrum,Prec> operator()(Ray in) const
        {
            return {Spectrum{},2.0};
        }

        std::pair<Spectrum,Prec> operator()(Ray* out,Vec2 rand) const
        {
            return {Spectrum{},2.0};
        }

        std::pair<Spectrum,Prec> operator()(Ray* out,Vec3 rand) const
        {
            return {Spectrum{},2.0};
        }
    };

    static_assert(plugins::DistributionFunction<test>);
}