#include "jpc_tracer/maths/maths.h"
#include "jpc_tracer/shaders/Concepts.h"

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

    static_assert(shader::DistributionFunction<test>);
}