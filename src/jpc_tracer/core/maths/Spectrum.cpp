#include "Spectrum.h"
#include "jpc_tracer/core/maths/Vec.h"
#include <functional>



namespace jpctracer
{
    Vec3 ToRGB(const Spectrum& spec) 
    {
        return Vec3{spec[0],spec[1],spec[2]};
    }
    

    Prec& Transparency(Spectrum& spec) 
    {
        return spec[4];
    }
    
    bool IsBlack(Spectrum spec) 
    {
        return std::abs(spec[0])<0.0001 
            && std::abs(spec[1])<0.0001 
            && std::abs(spec[2])<0.0001;
    }
    
    Spectrum Black() 
    {
        return Spectrum{0,0,0,1};
    }

    Spectrum FromValue(const Prec& v)
    {
        return Spectrum{v,v,v,v};
    }

    namespace math
    {
        void operator*=(Spectrum& spec1,const Spectrum& spec2) 
        {
            math::detail::VecMutableOp(spec1, spec2, std::multiplies<Prec>());
        }

        Spectrum operator*(const Spectrum& spec1,const Spectrum& spec2) 
        {
            return math::detail::VecImmutableOp(spec1,spec2, std::multiplies<Prec>());
        }
    }

}